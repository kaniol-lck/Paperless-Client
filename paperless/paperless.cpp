#include "paperless.h"

Paperless::Paperless(QObject *parent) :
    QObject(parent),
    api_(PaperlessApi::api())
{}

// Paperless *Paperless::client()
// {
//     static Paperless c;
//     return &c;
// }
