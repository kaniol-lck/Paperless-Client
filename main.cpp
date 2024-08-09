#include "client.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("kaniol");
    a.setApplicationName("Paperless Client");
    a.setApplicationDisplayName(QObject::tr("Paperless Client"));
    Client w;
    w.show();
    return a.exec();
}
