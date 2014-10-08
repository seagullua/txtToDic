#include "txtToDic.h"
#include <QFile>
#include <QTextStream>
typedef QStringList Array;
QString readFile(const QString& file_name)
{
    QFile file(file_name);
    file.open(QIODevice::ReadOnly);
    QString s = file.readAll();

    return s;
}

QStringList processString(const QString& input)
{
    QString string = input;
    string = string.replace(QRegExp("[0-9]+"), "");
    Array vec = string.toLower().split(QRegExp("\\W*\\s\\W*"));
    return vec;
}

Array makeUnique(Array array)
{
    array.removeDuplicates();
    array.sort();
    return array;
}

QStringList createDictionaryFromFile(const QString& file_name, int& words)
{
    QString s = readFile(file_name);
    Array vec = processString(s);
    words = vec.size();
    //vec.removeDuplicates();
    //vec.sort();
    return vec;
}

void addToDictionary(Dictionary& target, const Dictionary& source)
{
    target.append(source);
    //target.removeDuplicates();
    //target.sort();
}

void saveDictionaryToFile(Dictionary& d, const QString& file_name)
{
    d.removeDuplicates();
    d.sort();
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

QStringList removeSpaces(QStringList dictionary)
{
    QStringList no_space;
    foreach(QString word, dictionary)
    {
        if(word.trimmed().size() !=0 )
        {
            no_space.push_back(word);
        }
    }
    return no_space;
}
