#include "configtype.h"
#include "qboxlayout.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QToolButton>
#include <QFileDialog>
#include <QSpinBox>

Config::Config() {}

void Config::applyAttr(QWidget *widget)
{
    if(attrSetter_) attrSetter_(widget);
}

void Config::setAttr(const std::function<void (QWidget *)> &newAttrSetter)
{
    attrSetter_ = newAttrSetter;
}

std::tuple<QWidget *, std::function<QVariant ()> > LineEditConfig::configWidget(QWidget *parentWidget, const QVariant &value)
{
    auto line = new QLineEdit(parentWidget);
    line->setFrame(false);
    auto text = value.toString();
    line->setText(text);
    applyAttr(line);
    return { line, [line]{
                return line->text();
            }};
}


std::tuple<QWidget *, std::function<QVariant ()> > SpinBoxConfig::configWidget(QWidget *parentWidget, const QVariant &value)
{
    auto spinBox = new QSpinBox(parentWidget);
    spinBox->setFrame(false);
    auto val = value.toInt();
    spinBox->setValue(val);
    applyAttr(spinBox);
    return { spinBox, [spinBox]{
                return spinBox->value();
            }};
}

std::tuple<QWidget *, std::function<QVariant ()> > CheckBoxConfig::configWidget(QWidget *parentWidget, const QVariant &value)
{
    auto checkBox = new QCheckBox(parentWidget);
    checkBox->setChecked(value.toBool());
    applyAttr(checkBox);
    return { checkBox, [checkBox]{
                return checkBox->isChecked();
            } };
}

ComboBoxConfig::ComboBoxConfig(QStringList items) :
    items_(items)
{}

std::tuple<QWidget *, std::function<QVariant ()> > ComboBoxConfig::configWidget(QWidget *parentWidget, const QVariant &value)
{
    auto comboBox = new QComboBox(parentWidget);
    comboBox->addItems(items_);
    comboBox->setCurrentIndex(value.toInt());
    applyAttr(comboBox);
    return { comboBox, [comboBox]{
                return comboBox->currentIndex();
            }};
}

FilePathConfig::FilePathConfig(const QString &caption, const QString &filter) :
    caption_(caption),
    filter_(filter)
{}

std::tuple<QWidget *, std::function<QVariant ()> > FilePathConfig::configWidget(QWidget *parentWidget, const QVariant &value)
{
    auto widget = new QWidget(parentWidget);
    auto layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    auto line = new QLineEdit(parentWidget);
    line->setFrame(false);
    layout->addWidget(line);
    auto button = new QToolButton(parentWidget);
    button->setText("...");
    layout->addWidget(button);
    QObject::connect(button, &QToolButton::clicked, parentWidget, [=, caption = caption_, filter = filter_]{
        auto fileName = QFileDialog::getOpenFileName(parentWidget, caption, line->text(), filter);
        if(fileName.isEmpty()) return;
        line->setText(fileName);
    });

    auto text = value.toString();
    line->setText(text);
    applyAttr(widget);
    return { widget, [line]{
                return line->text();
            }};
}
