#include "txtToDic.h"
#include <QFile>
#include <QTextStream>
typedef QStringList Array;
QStringList createDictionaryFromFile(const QString& file_name, int& words)
{
    QFile file(file_name);
    file.open(QIODevice::ReadOnly);
    QString s = file.readAll();

    s = s.toLower();
    Array vec = s.split(QRegExp("\\W*\\s\\W*"));
    words = vec.size();
    vec.removeDuplicates();
    vec.sort();
    return vec;
}

void addToDictionary(Dictionary& target, const Dictionary& source)
{
    target.append(source);
    target.removeDuplicates();
    target.sort();
}

void saveDictionaryToFile(const Dictionary& d, const QString& file_name)
{
    QString output = d.join("\n");
    QString number;
    if(d[0] != "")
    {
        number = QString::number(d.size()) + "\n";
    }
    else
    {
        number = QString::number(d.size()-1);
    }
    QFile file(file_name);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out << number << output;
}


