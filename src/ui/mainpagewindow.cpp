#include "mainpagewindow.h"
#include "ui_mainpagewindow.h"

#include <QPainter>
#include <QPushButton>

MainPageWindow::MainPageWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainPageWindow)
{
    ui->setupUi(this);
    setMouseTracking(true);
    setWindowTitle(tr("Welcome Page"));
    button_ = new QPushButton(this);
    button_->setText(tr("Exit"));
    connect(button_, &QPushButton::clicked, this, []{
        qApp->exit();
    });
}

MainPageWindow::~MainPageWindow()
{
    delete ui;
}

void MainPageWindow::paintEvent(QPaintEvent *event)
{
    button_->setGeometry(width() - 100, height() - 60, 80, 40);
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
