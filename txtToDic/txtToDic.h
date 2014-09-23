#ifndef TXTTODIC_H
#define TXTTODIC_H

#include <QStringList>
typedef QStringList Dictionary;

QString readFile(const QString& file_name);
QStringList processString(const QString& string);
Dictionary createDictionaryFromFile(const QString& file_name, int& words);
void addToDictionary(Dictionary& target, const Dictionary& source);
void saveDictionaryToFile(Dictionary& d, const QString& file_name);
QStringList makeUnique(QStringList array);
QStringList removeSpaces(QStringList);

#endif // TXTTODIC_H
