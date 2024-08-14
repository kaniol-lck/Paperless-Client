#include "paperless.h"

Paperless::Paperless(QObject *parent) :
    QObject(parent),
    api_(new PaperlessApi(this))
{}

PaperlessApi *Paperless::api() const
{
    return api_;
}

// Paperless *Paperless::client()
// {
//     static Paperless c;
//     return &c;
// }
