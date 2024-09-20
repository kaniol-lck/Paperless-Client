#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>

class ImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageWidget(QWidget *parent = nullptr);

    void setImage(const QImage &newImage);

signals:
protected:
    void paintEvent(QPaintEvent *event) override;
    QImage image_;
};

#endif // IMAGEWIDGET_H
