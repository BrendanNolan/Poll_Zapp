#include "MainWindow.h"

#include <QFileInfo>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QPushButton>
#include <QVector>

#include <chrono>

#include "ConstituencyExplorerWidget.h"
#include "ConstituencyModel.h"
#include "ConstituencyPixmapProxyModel.h"
#include "ElectionDefinitions.h"
#include "ElectionGuiFunctions.h"
#include "PoliticianModel.h"
#include "PoliticianPictureProxyModel.h"
#include "RotatingItemsWidget.h"
#include "SqlDatabaseManagerFactory.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , constituencyExplorerWidget_(new ConstituencyExplorerWidget)
    , rotatingItemsLoadScreen_(new RotatingItemsWidget(this))
{
    rotatingItemsLoadScreen_->setWindowModality(Qt::ApplicationModal);
    rotatingItemsLoadScreen_->hide();
    setCentralWidget(constituencyExplorerWidget_);

    election_gui_functions::runPythonScript(QFileInfo(paths::scraperScript));

    auto factory = SqlDatabaseManagerFactory(QFileInfo(
        paths::databasePath));

    politicianModel_ = new PoliticianModel(factory, this);
    constituencyModel_ = new ConstituencyModel(factory, this);
    auto constituencyProxyModel = new ConstituencyPixmapProxyModel(
        this, constituencyModel_, politicianModel_);
    auto constituencySelectionModel = new QItemSelectionModel(
        constituencyProxyModel);

    auto politicianProxyModel = new PoliticianPictureProxyModel(
        this, politicianModel_);
    auto politicianSelectionModel = new QItemSelectionModel(
        politicianProxyModel);

    constituencyExplorerWidget_->setPoliticianModel(politicianProxyModel);
    constituencyExplorerWidget_->setPoliticianSelectionModel(
        politicianSelectionModel);
    constituencyExplorerWidget_->setConstituencyModel(constituencyProxyModel);
    constituencyExplorerWidget_->setConstituencySelectionModel(
        constituencySelectionModel);

    auto refreshDataButton = new QPushButton("Refresh Data");
    constituencyExplorerWidget_->buttonLayout()->addWidget(refreshDataButton);
    connect(refreshDataButton, &QPushButton::clicked,
        this, &MainWindow::asynchronouslyRefreshData);

    connect(&dataRefreshTimer_, &QTimer::timeout,
        this, &MainWindow::onDataRefreshTimerTimeout);
}

void MainWindow::refreshData()
{
    mutex_.lock();
    election_gui_functions::runPythonScript(QFileInfo(paths::scraperScript));
    mutex_.unlock();
    refreshModels();
}

void MainWindow::asynchronouslyRefreshData()
{
    rotatingItemsLoadScreen_->show();
    dataRefreshTimer_.setInterval(std::chrono::seconds(1));
    fut_ = std::async(std::launch::async, &MainWindow::refreshData, this);
    dataRefreshTimer_.start();
}

void MainWindow::onDataRefreshTimerTimeout()
{
    auto status = fut_.wait_for(std::chrono::seconds(0));
    if (status == std::future_status::ready)
    {
        dataRefreshTimer_.stop();
        rotatingItemsLoadScreen_->hide();
    }
}

void MainWindow::refreshModels()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (constituencyModel_)
        constituencyModel_->refresh();
    if (politicianModel_)
        politicianModel_->refresh();
}
