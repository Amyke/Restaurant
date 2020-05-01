#ifndef WORKSPACEWIDGET_HPP
#define WORKSPACEWIDGET_HPP

#include <QtWidgets/QSplitter>

class Model;
class OrdersModel;

class QStackedWidget;

class WorkspaceWidget : public QSplitter {
    Q_OBJECT
public:
    explicit WorkspaceWidget(Model &model, QWidget *parent = nullptr);

public slots:
    void requestAdministration();
    void requestDiagramm();
    void requestOrderDetails();

private:
    Model &model_;
    QStackedWidget *toolbox_;

    OrdersModel *ordersModel_;
};

#endif //! WORKSPACEWIDGET_HPP
