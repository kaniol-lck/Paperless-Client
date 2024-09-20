#include "mainpagewindow.h"
#include "ui_mainpagewindow.h"

#include <QPainter>

MainPageWindow::MainPageWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainPageWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Welcome Page"));
    // setStyleSheet("background-image: url(background.jpg);");
}

MainPageWindow::~MainPageWindow()
{
    delete ui;
}

void MainPageWindow::paintEvent(QPaintEvent *event)
// void MainPageWindow::resizeEvent(QResizeEvent *event)
{
    static QImage image("background.jpg");
    auto image2 = image.scaled(size(), Qt::KeepAspectRatioByExpanding);
    auto r = double(image2.height()) / image.height();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    auto re = rect();
    re.setSize(image2.size());
    re.translate((width() - image2.width()) / 2 * r,
                 (height() - image2.height()) / 2 * r);
    painter.drawImage(re, image);
}
