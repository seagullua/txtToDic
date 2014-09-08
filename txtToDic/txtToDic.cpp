#include "txtToDic.h"
#include <QFile>

QStringList createDictionaryFromFile(const QString& file_name)
{
    QFile file(file_name);
    file.open(QIODevice::ReadOnly);
    QString s = file.readAll();

    s = s.toLower();
    QStringList vec = s.split(QRegExp("\\W*\\s\\W*"));
    vec.removeDuplicates();
    vec.sort();
    return vec;
}

void addToDictionary(Dictionary& target, const Dictionary& source)
{
    target.append(source);
    target.removeDuplicates();
    target.sort();
    return target;
}

