#ifndef PoliticianListWidget_H
#define PoliticianListWidget_H

#include <QWidget>

class ConstituencyWidget;
class PoliticianModel;
class QItemSelectionModel;

namespace Ui
{
    class PoliticianListWidget;
}

class PoliticianListWidget : public QWidget
{
public:
    PoliticianListWidget(
        QWidget* parent = nullptr,
        Qt::WindowFlags flags = Qt::WindowFlags());
    ~PoliticianListWidget();

    void setModel(PoliticianModel* model);
    void setSelectionModel(QItemSelectionModel* selectionModel);

signals:
    void politicianClicked(const QModelIndex& index);

public slots:
    void setConstituency(int constituencyId);

private:
    PoliticianModel* politicianModel_;
    QItemSelectionModel* politicianSelectionModel_; // May not need to keep this here. May end up just giving the selection model to ui_->politicianListView
    Ui::PoliticianListWidget* ui_;
};

#endif // PoliticianListWidget_H
