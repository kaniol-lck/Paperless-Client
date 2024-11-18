#ifndef CUSTOMFIELDMODEL_H
#define CUSTOMFIELDMODEL_H

#include "PaperlessModel.hpp"
#include "paperless/customField.h"

class CustomFieldModel : public PaperlessModel<CustomField>
{
    Q_OBJECT
public:
    CustomFieldModel(QObject *parent, Paperless *client);

    enum Column{
        IdColumn,
        NameColumn,
        DataTypeColumn,
        ColumnSize
    };

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

#endif // CUSTOMFIELDMODEL_H
