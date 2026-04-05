#ifndef MAINWINDOWPANEL_H
#define MAINWINDOWPANEL_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindowPanel;
}
QT_END_NAMESPACE

class MainWindowPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowPanel(QWidget *parent = nullptr);
    ~MainWindowPanel() override;

private:
    Ui::MainWindowPanel *ui;
};
#endif // MAINWINDOWPANEL_H
