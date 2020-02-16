#include "ConstituencyDrillDownWidget.h"
#include "ui_ConstituencyDrillDownWidget.h"

#include <QtGlobal>

#include <QItemSelectionModel>
#include <QListView>
#include <QRadioButton>

#include "ConstituencyModel.h"
#include "PoliticianDelegate.h"
#include "PoliticianModel.h"
#include "PoliticianPictureProxyModel.h"

ConstituencyDrillDownWidget::ConstituencyDrillDownWidget(
    QWidget* parent, 
    Qt::WindowFlags flags)
    : QWidget(parent, flags)
    , ui_(new Ui::ConstituencyDrillDownWidget)
    , politicianProxyModel_(nullptr)
{
    ui_->setupUi(this);
    ui_->mpsRadioButton->setChecked(true);
    ui_->candidatesRadioButton->setChecked(false);
    ui_->politicianListView->setItemDelegate(new PoliticianDelegate(this));
}

ConstituencyDrillDownWidget::~ConstituencyDrillDownWidget()
{
    delete ui_;
}

void ConstituencyDrillDownWidget::setPoliticianModel(PoliticianPictureProxyModel* model)
{
    politicianProxyModel_ = model;
    ui_->politicianListView->setModel(model);
    loadConstituency();

    auto& politicianModel = *(politicianProxyModel_->politicianModel());
    if (ui_->mpsRadioButton->isChecked())
    {
        politicianModel.setElectoralStatus(
            PoliticianModel::ElectoralStatus::SITTING);
    }
    else if (ui_->candidatesRadioButton->isChecked())
    {
        politicianModel.setElectoralStatus(
            PoliticianModel::ElectoralStatus::CANDIDATE);
    }
    connect(ui_->mpsRadioButton, &QRadioButton::toggled,
        [this](bool checked) {
            if (!checked)
                return;
            politicianProxyModel_->politicianModel()->setElectoralStatus(
                PoliticianModel::ElectoralStatus::SITTING);
        });
    connect(ui_->candidatesRadioButton, &QRadioButton::toggled,
        [this](bool checked) {
            if (!checked)
                return;
            politicianProxyModel_->politicianModel()->setElectoralStatus(
                PoliticianModel::ElectoralStatus::CANDIDATE);
        });
}

void ConstituencyDrillDownWidget::setPoliticianSelectionModel(
    QItemSelectionModel* selectionModel)
{
    Q_ASSERT(selectionModel->model() == politicianProxyModel_);
    ui_->politicianListView->setSelectionModel(selectionModel);
}

void ConstituencyDrillDownWidget::setConstituencyModel(ConstituencyModel* model)
{
    constituencyModel_ = model;
    loadConstituency();
    connect(model, &QAbstractItemModel::dataChanged,
        this, &ConstituencyDrillDownWidget::onConstituencyDataChanged);
}

void ConstituencyDrillDownWidget::onConstituencyDataChanged(
    const QModelIndex& topLeft,
    const QModelIndex& bottomRight)
{
    auto toDisplay = constituencyToDisplay();
    if (!toDisplay.isValid())
    {
        setToInvalidState();
        return;
    }
    if (QItemSelectionRange(topLeft, bottomRight).contains(toDisplay))
        loadConstituency();
}

void ConstituencyDrillDownWidget::setConstituencySelectionModel(
    QItemSelectionModel* selectionModel)
{
    constituencySelectionModel_ = selectionModel;
    loadConstituency();
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
        [this]() { loadConstituency(); });
}

QModelIndexList ConstituencyDrillDownWidget::selectedPoliticians() const
{
    return ui_->politicianListView->selectionModel()->selectedRows();
}

QModelIndex ConstituencyDrillDownWidget::constituencyToDisplay() const
{
    if (!constituencyModel_) 
        return QModelIndex();
    if (!constituencySelectionModel_)
        return constituencyModel_->index(0);
    auto indexList = constituencySelectionModel_->selectedIndexes(); 
    if (indexList.isEmpty())
        return constituencyModel_->index(0);
    return indexList.first();
}

void ConstituencyDrillDownWidget::loadConstituency()
{ 
    auto constituencyIndex = constituencyToDisplay();
    if (!constituencyIndex.isValid() || !politicianProxyModel_)
    {
        setToInvalidState();
        return;
    }

    enableRadioButtons();
    
    auto constituencyName = constituencyModel_->data(
        constituencyIndex, ConstituencyModel::NameRole).toString();
    ui_->constituencyLabel->setText(constituencyName);

    auto constituencyId = constituencyModel_->data(
        constituencyIndex, ConstituencyModel::IdRole).toInt();
    politicianProxyModel_->politicianModel()->setConstituency(constituencyId);
}

void ConstituencyDrillDownWidget::setToInvalidState()
{
    if (politicianProxyModel_)
        politicianProxyModel_->politicianModel()->setConstituency(-1);
    ui_->constituencyLabel->setText("<Constituency Name>");
    disableRadioButtons();
}

void ConstituencyDrillDownWidget::enableRadioButtons()
{
    ui_->mpsRadioButton->setEnabled(true);
    ui_->candidatesRadioButton->setEnabled(true);
}

void ConstituencyDrillDownWidget::disableRadioButtons()
{
    ui_->mpsRadioButton->setDisabled(true);
    ui_->candidatesRadioButton->setDisabled(true);
}
