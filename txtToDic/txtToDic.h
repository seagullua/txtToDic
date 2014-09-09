#ifndef TXTTODIC_H
#define TXTTODIC_H

#include <QStringList>
typedef QStringList Dictionary;

Dictionary createDictionaryFromFile(const QString& file_name, int& words);
void addToDictionary(Dictionary& target, const Dictionary& source);
void saveDictionaryToFile(Dictionary& d, const QString& file_name);

#endif // TXTTODIC_H
