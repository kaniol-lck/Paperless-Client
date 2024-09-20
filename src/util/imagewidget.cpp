#include "imagewidget.h"

#include <QPainter>

ImageWidget::ImageWidget(QWidget *parent)
    : QWidget{parent}
{
    setStyleSheet("QWidget{ background-color: #333;}");
}

void ImageWidget::paintEvent(QPaintEvent *event)
{
    auto image2 = image_.scaled(size(), Qt::KeepAspectRatio);
    auto r = double(image2.height()) / image_.height();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    auto re = rect();
    re.setSize(image2.size());
    re.translate((width() - image2.width()) / 2 * r,
                 (height() - image2.height()) / 2 * r);

    painter.fillRect(rect(), QColor("#333"));
    painter.drawImage(re, image_);
    QWidget::paintEvent(event);
}

void ImageWidget::setImage(const QImage &newImage)
{
    image_ = newImage;
}
