#ifndef PAIRSTORAGE_H
#define PAIRSTORAGE_H

#include <QString>
#include <QSet>
#include <set>

template <class A, class B>
struct TermDocGeneric
{
    TermDocGeneric(A t, B id)
        : term(t), docid(id)
    {}
    A term;
    B docid;

    bool operator<(const TermDocGeneric<A, B>& b) const
    {
        return term < b.term || (term == b.term && docid < b.docid);
    }
};
typedef TermDocGeneric<int, int> TermDoc;
typedef std::set<TermDoc> TermDocChunk;

class DictionaryBuilder;

class PairStorage
{
public:
    PairStorage(QString output_file_name, int max_size, DictionaryBuilder& builder);
    void readFile(QString path, int id);
    ~PairStorage();
private:
    void flushToFile();
    void flushIfNeeded();

    TermDocChunk _chunk;
    int _max_size;
    QString _output_file_name;
    int _file_index;
    DictionaryBuilder& _builder;
};

void processDocuments(QString dir, QString output_dir, int max_size, int threads);

#endif // PAIRSTORAGE_H
