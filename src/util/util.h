#ifndef UTIL_H
#define UTIL_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QIcon>

void openFolder(const QString &path);
void openFileInFolder(const QString &filePath);
void openFileInFolder(const QString &fileName, const QString &filePath);

QIcon reverseIcon(const QIcon& icon, int extent = 32);

QDialog *makeDialog(QWidget *widget, const QString &title = "");

#endif // UTIL_H
