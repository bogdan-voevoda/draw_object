#include <QtGui>
#include <QMessageBox>
#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if ( ((QKeyEvent*)event)->key() == Qt::Key_S || ((QKeyEvent*)event)->key() == Qt::Key_W ||
         ((QKeyEvent*)event)->key() == Qt::Key_A || ((QKeyEvent*)event)->key() == Qt::Key_D ) {

        if ( event->type() == QEvent::KeyPress ) {
            if ( ((QKeyEvent*)event)->key() == Qt::Key_A ) { isMoveLeft = true; }
            if ( ((QKeyEvent*)event)->key() == Qt::Key_D ) { isMoveRight = true; }
            if ( ((QKeyEvent*)event)->key() == Qt::Key_W ) { isMoveUp = true; }
            if ( ((QKeyEvent*)event)->key() == Qt::Key_S ) { isMoveDown = true; }
        } else if (event->type() == QEvent::KeyRelease) {
            if ( ((QKeyEvent*)event)->key() == Qt::Key_A ) { isMoveLeft = false; }
            if ( ((QKeyEvent*)event)->key() == Qt::Key_D ) { isMoveRight = false; }
            if ( ((QKeyEvent*)event)->key() == Qt::Key_W ) { isMoveUp = false; }
            if ( ((QKeyEvent*)event)->key() == Qt::Key_S ) { isMoveDown = false; }
        }
    }
    return false;
}

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    this->installEventFilter(this);

    ui->setupUi(this);
    centralWgt = new QWidget;
    solutionView = new MainWidget(this);
    centralWgt->setLayout(ui->gridLayout);
    this->setCentralWidget(centralWgt);
    ui->view->addWidget(solutionView);

    QWidget *topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

#ifdef Q_OS_SYMBIAN
    infoLabel = new QLabel(tr("<i>Choose a menu option</i>"));
#else
    infoLabel = new QLabel(tr("<i>Choose a menu option, or right-click to "
                              "invoke a context menu</i>"));
#endif
    infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    infoLabel->setAlignment(Qt::AlignCenter);

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(5);
    layout->addWidget(topFiller);
    layout->addWidget(infoLabel);
    layout->addWidget(bottomFiller);
    createActions();
    createMenus();

    setMinimumSize(160, 160);
    resize(840, 580);

    startTimer(500);
}

void MainWindow::timerEvent(QTimerEvent *) {
    QString str;
    str.setNum((int)solutionView->getFPS());
    this->ui->lineEdit->setText(str);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    menu.exec(event->globalPos());
}

void MainWindow::newFile() {
    infoLabel->setText(tr(" <b>File|New</b>"));
}

void MainWindow::scalePlus() {
    infoLabel->setText(tr(" <b>+</b>"));
    solutionView->updateGL();
}

void MainWindow::scaleMinus() {
    infoLabel->setText(tr(" <b>-</b>"));
    solutionView->updateGL();
}

void MainWindow::open() {
    infoLabel->setText(tr("Invoked <b>File|Open</b>"));
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                                                    tr("Text files (*.txt)"));
}

void MainWindow::save() {
    QString fileName = QFileDialog::getSaveFileName(0,"Save file",QDir::currentPath(),
                                                    "Text files (*.txt);;All files (*.*)",
                                                        new QString("Text files (*.txt)"));
    infoLabel->setText(tr("Invoked <b>File|Save</b>"));
}


void MainWindow::alpha_on() {
    solutionView->isAlphaBlendingOn = !solutionView->isAlphaBlendingOn;
    solutionView->updateGL();
}

void MainWindow::culling_on() {
    solutionView->isCullingOn = !solutionView->isCullingOn;
    solutionView->updateGL();
}

void MainWindow::light_cheme_change() {
    solutionView->lightCase = !solutionView->lightCase;
    solutionView->updateGL();

    solutionView->obj->isPending = !solutionView->obj->isPending;
}

void MainWindow::on_pushButton_3_pressed() {
    if (solutionView->obj) {
        solutionView->delObj();
        this->ui->pushButton_3->setText("Create Cubes");
    } else {
        solutionView->createObj();
        this->ui->pushButton_3->setText("Delete Cubes");
    }
}

void MainWindow::wire_mode() {
    solutionView->isWireMode = !solutionView->isWireMode;
    solutionView->updateGL();
}

void MainWindow::on_textCanged() {}

void MainWindow::createActions() {
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    scalePlusAct = new QAction(tr("&+"), this);
    scalePlusAct->setStatusTip(tr("Scale +"));
    connect(scalePlusAct, SIGNAL(triggered()), this, SLOT(scalePlus()));

    scaleMinusAct = new QAction(tr("&-"), this);
    scaleMinusAct->setStatusTip(tr("Scale -"));
    connect(scaleMinusAct, SIGNAL(triggered()), this, SLOT(scaleMinus()));

    AlphaOn = new QAction(tr("&Alpha"), this);
    AlphaOn->setStatusTip(tr("Alpha"));
    connect(AlphaOn, SIGNAL(triggered()), this, SLOT(alpha_on()));

    CullingOn = new QAction(tr("&Culling"), this);
    CullingOn->setStatusTip(tr("Culling"));
    connect(CullingOn, SIGNAL(triggered()), this, SLOT(culling_on()));

    LightScheme = new QAction(tr("&Light Scheme"), this);
    LightScheme->setStatusTip(tr("Light Scheme"));
    connect(LightScheme, SIGNAL(triggered()), this, SLOT(light_cheme_change()));

    WireMode = new QAction(tr("&Wire Mode"), this);
    WireMode->setStatusTip(tr("Wire Mode"));
    connect(WireMode, SIGNAL(triggered()), this, SLOT(wire_mode()));
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(scalePlusAct);
    fileMenu->addAction(scaleMinusAct);
    fileMenu->addAction(AlphaOn);
    fileMenu->addAction(CullingOn);
    fileMenu->addAction(LightScheme);
    fileMenu->addAction(WireMode);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
}

MainWindow::~MainWindow() {
    delete solutionView;
    delete centralWgt;
    delete ui;
}
