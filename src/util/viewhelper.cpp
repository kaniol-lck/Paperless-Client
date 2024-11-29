#include "viewhelper.h"

#include <QElapsedTimer>
#include <QEvent>
#include <QTime>
#include <QTimer>

ViewHelper::ViewHelper(QAbstractItemView *view, const QModelIndex &index) :
    QObject(view),
    view_(view),
    index_(index)
{
    view->installEventFilter(this);
}

bool ViewHelper::eventFilter(QObject *watched, QEvent *event)
{
    auto &&model = view_->model();
    auto viewportRect = view_->viewport()->rect();
    if(event->type() == QEvent::Type::Paint){
        QElapsedTimer timer;
        timer.start();
        for(int row = 0; row < model->rowCount(index_); row++){
            for(auto &&[column, creator] : columnCreator_.asKeyValueRange()){
                // qDebug() << column << "render";
                auto index = model->index(row, column, index_);
                if(!enabled_){
                    view_->setIndexWidget(index, nullptr);
                    continue;
                }
                bool hasWidget = view_->indexWidget(index);
                bool intersects = view_->visualRect(index).intersects(viewportRect);
                if(!hasWidget && intersects){
                    auto widget = columnCreator_[column](index, model);
                    view_->setIndexWidget(index, widget);
                } else if(hasWidget && !intersects){
                    view_->setIndexWidget(index, nullptr);
                }
            }
            // qDebug() << "cost" << timer.elapsed();
        }
    }
    return QObject::eventFilter(watched, event);
}

void ViewHelper::addCreator(int column, const std::function<QWidget *(const QModelIndex &, QAbstractItemModel *)> &newCreator)
{
    columnCreator_[column] = newCreator;
}

void ViewHelper::removeCreator(int column)
{
    columnCreator_.remove(column);
}

bool ViewHelper::enabled() const
{
    return enabled_;
}

void ViewHelper::setEnabled(bool newEnabled)
{
    enabled_ = newEnabled;
}

void ViewHelper::setIndex(const QModelIndex &newIndex)
{
    index_ = newIndex;
}

QModelIndexList ViewHelper::renderedIndexes()
{
    QModelIndexList list;
    QRegion indexRegion;
    auto rect = view_->viewport()->rect();
    for(int x = 0; x < rect.width(); x++){
        for(int y = 0; y < rect.height(); y++){
            QPoint pos(x, y);
            // qDebug() << rect << pos;
            if(indexRegion.contains(pos)) continue;
            if(auto index = view_->indexAt(pos);
                index.isValid()){
                list << index;
                indexRegion += view_->visualRect(index);
            }
        }
    }
    // qDebug() << list;
    return list;
}
