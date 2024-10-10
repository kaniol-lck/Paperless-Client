#ifndef CSVHELPER_HPP
#define CSVHELPER_HPP

#include "util.hpp"

#include <QFile>

void toCSV(QList<QVariant> list)
{

}

QPair<QStringList, QList<QStringList>> fromCSVV(const QString &fileName, int header, int data){
    QList<QStringList> list;
    QFile file(fileName);
    QTextStream in(&file);
    if(!file.open(QIODevice::ReadOnly)) return {};

    QString string;

    bool inQuote = false;
    // QList<QStringList> data;
    QStringList headerLine;
    QStringList line;
    QString value;

    int lineCount = 0;
    while(in.readLineInto(&string)){
        string.append('\n');
        for (int i = 0; i < string.size(); i++) {
            QChar current = string.at(i);

            // Normal
            if (!inQuote) {
                // newline
                if (current == '\n') {
                    // add value
                    line << value;
                    value.clear();
                    // add line
                    if(lineCount == header)
                        headerLine = line;
                    else if(lineCount >= data){
                        list << line;
                    }
                    line.clear();
                }
                // comma
                else if (current == ',') {
                    // add line
                    line << value;
                    value.clear();
                }
                // double quote
                else if (current == '"') {
                    inQuote = true;
                }
                // character
                else {
                    value += current;
                }
            }
            // Quote
            else if (inQuote) {
                // double quote
                if (current == '"') {
                    int index = (i+1 < string.size()) ? i+1 : string.size();
                    QChar next = string.at(index);
                    if (next == '"') {
                        value += '"';
                        i++;
                    } else {
                        inQuote = false;
                    }
                }
                // other
                else {
                    value += current;
                }
            }
        }
        lineCount++;
    }
    // qDebug() << headerLine << list;
    return qMakePair(headerLine, list);
}

QList<QVariant> fromCSV(const QString &fileName, int header, int data){
    QVariantList list;
    QFile file(fileName);
    QTextStream in(&file);
    if(!file.open(QIODevice::ReadOnly)) return {};

    QString string;

    bool inQuote = false;
    // QList<QStringList> data;
    QStringList headerLine;
    QStringList line;
    QString value;

    int lineCount = 0;
    while(in.readLineInto(&string)){
        string.append('\n');
        for (int i = 0; i < string.size(); i++) {
            QChar current = string.at(i);

            // Normal
            if (!inQuote) {
                // newline
                if (current == '\n') {
                    // add value
                    line << value;
                    value.clear();
                    // add line
                    if(lineCount == header)
                        headerLine = line;
                    else if(lineCount >= data){
                        // QVariant v;
                        auto v = make(headerLine, line);
                        // qDebug() << v;
                        // for(int j = 0; j < headerLine.count(); j++){
                            // auto key = headerLine.at(j);
                            // auto value = line.at(j);
                            // insert(v, key, value);
                        // }

                        list << v;
                        // list << line;
                    }
                    line.clear();
                }
                // comma
                else if (current == ',') {
                    // add line
                    line << value;
                    value.clear();
                }
                // double quote
                else if (current == '"') {
                    inQuote = true;
                }
                // character
                else {
                    value += current;
                }
            }
            // Quote
            else if (inQuote) {
                // double quote
                if (current == '"') {
                    int index = (i+1 < string.size()) ? i+1 : string.size();
                    QChar next = string.at(index);
                    if (next == '"') {
                        value += '"';
                        i++;
                    } else {
                        inQuote = false;
                    }
                }
                // other
                else {
                    value += current;
                }
            }
        }
        lineCount++;
    }
    qDebug() << headerLine << list;
    return list;
}

#endif // CSVHELPER_HPP
