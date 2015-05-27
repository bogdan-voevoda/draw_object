#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QActionGroup>
#include <mainwidget.h>

class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class MainWidget;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    Ui::MainWindow  *ui;
    QWidget         *centralWgt;
    MainWidget      *solutionView;
    QMenu           *fileMenu;
    QAction         *newAct;
    QAction         *openAct;
    QAction         *saveAct;
    QAction         *exitAct;
    QAction         *scalePlusAct;
    QAction         *scaleMinusAct;
    QAction         *AlphaOn;
    QAction         *CullingOn;
    QAction         *LightScheme;
    QAction         *WireMode;
    QLabel          *infoLabel;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool isMoveUp = false;
    bool isMoveDown = false;
    bool isMoveLeft = false;
    bool isMoveRight = false;

protected:
    void createMenus();
    void createActions();
    void timerEvent(QTimerEvent *);
    bool eventFilter(QObject *obj, QEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
     void newFile();
     void open();
     void save();
     void on_textCanged();
     void scalePlus();
     void scaleMinus();
     void wire_mode();
     void alpha_on();
     void culling_on();
     void light_cheme_change();
     void on_pushButton_3_pressed();
};

#endif // MAINWINDOW_H
