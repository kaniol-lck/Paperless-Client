#ifndef CONFIGTYPE_H
#define CONFIGTYPE_H

#include <QVariant>

class QWidget;

class Config
{
public:
    Config();
    virtual ~Config() = default;

    virtual std::tuple<QWidget*, std::function<QVariant()>> configWidget(QWidget *parentWidget, const QVariant &value) = 0;

    void applyAttr(QWidget *widget);
    void setAttr(const std::function<void (QWidget *)> &newAttrSetter);

private:
    std::function<void (QWidget*)> attrSetter_;
};

class QLineEdit;
class LineEditConfig : public Config
{
public:
    std::tuple<QWidget*, std::function<QVariant()>> configWidget(QWidget *parentWidget, const QVariant &value) override;
};

class SpinBoxConfig : public Config
{
public:
    std::tuple<QWidget*, std::function<QVariant()>> configWidget(QWidget *parentWidget, const QVariant &value) override;
};

class CheckBoxConfig : public Config
{
public:
    std::tuple<QWidget*, std::function<QVariant()>> configWidget(QWidget *parentWidget, const QVariant &value) override;
};

class ComboBoxConfig : public Config
{
public:
    ComboBoxConfig(QStringList items);
    std::tuple<QWidget*, std::function<QVariant()>> configWidget(QWidget *parentWidget, const QVariant &value) override;

private:
    QStringList items_;
};

class FilePathConfig : public Config
{
public:
    FilePathConfig(const QString &caption = "", const QString &filter = "");
    std::tuple<QWidget*, std::function<QVariant()>> configWidget(QWidget *parentWidget, const QVariant &value) override;

private:
    QString caption_;
    QString filter_;
};

#endif // CONFIGTYPE_H
