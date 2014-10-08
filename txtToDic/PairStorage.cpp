#include "PairStorage.h"
#include <QFile>
#include <QTextStream>
#include "txtToDic.h"
#include <QDir>
#include <mutex>
#include <queue>
#include <QDebug>
#include <thread>
#include <QDataStream>
#include <sstream>
#include <QVector>
#include "stemming/russian_stem.h"
#include "stemming/english_stem.h"
#include <string>

#include "indexer.h"



class DictionaryBuilder
{
public:
    QVector<int> getIndexes(const QStringList& words)
    {
        QVector<int> res;
        res.resize(words.size());
        {
             std::unique_lock<std::mutex> lock(_mutex);
             for(int i=0; i<words.size(); ++i)
             {
                 res[i] = ::Index::getOrAddWordID(_index, words[i]);
             }
        }
        return res;

    }

    void saveToFile(QString filename)
    {
        QFile file( filename );
        if ( file.open(QIODevice::WriteOnly) )
        {
            QTextStream stream(&file);
            stream << _index.size() << endl;
            foreach(auto it, _index.keys())
            {
                stream << it << ' ' << _index[it] << endl;
            }
        }
    }

private:
    typedef QMap<QString, int> Index;
    Index _index;
    std::mutex _mutex;
};

PairStorage::PairStorage(QString output_file_name, int max_size, DictionaryBuilder& builder)
    : _file_index(0),
      _max_size(max_size),
      _output_file_name(output_file_name),
      _builder(builder)
{
}

void PairStorage::readFile(QString path, int id)
{
    int words = 0;
    QStringList list = createDictionaryFromFile(path,words);
    list.removeDuplicates();

    stemming::russian_stem<> Stem;
    stemming::english_stem<> Stem2;

    for(int i=0; i<list.size(); ++i)
    {
        QString word = list[i];

        std::wstring w = word.toStdWString();
        Stem(w);
        //Stem2(w);
        list[i] = QString::fromStdWString(w);
    }
    list.removeDuplicates();

    QVector<int> ids = _builder.getIndexes(list);

    for(int i=0; i<ids.size(); ++i)
    {
        if(list[i].size() > 0)
        {
            _chunk.insert(TermDoc(ids[i], id));
        }
    }
    flushIfNeeded();

}

PairStorage::~PairStorage()
{
    flushToFile();
}
void PairStorage::flushIfNeeded()
{
    if(_chunk.size() > _max_size)
    {
        flushToFile();
    }
}

void PairStorage::flushToFile()
{
    QString filename= _output_file_name + QString::number(_file_index++) + ".list";
    QFile file( filename );
    if ( file.open(QIODevice::WriteOnly) )
    {
        QByteArray device;
        QTextStream ss(&device);
        ss << _chunk.size() << endl;

        QTextStream stream(&file);
        foreach(const TermDoc& term_doc, _chunk)
        {
            ss << term_doc.term << " " << term_doc.docid << endl;
        }
        stream << device;
        _chunk.clear();
    }
    else
    {
        qDebug() << "Can't open file" << filename;
    }
}

struct FileID
{
    QString path;
    int id;
};

class FilesQueue
{
public:
    void addFile(const FileID& f)
    {
        _file_queue.push(f);
    }

    bool getFileThreadSafe(FileID& f)
    {
        bool found = false;
        {
            std::unique_lock<std::mutex> lock(_mutex);

            if(_file_queue.size() > 0)
            {
                f = _file_queue.front();
                _file_queue.pop();
                if(_file_queue.size() % 20 == 0)
                {
                    qDebug() << f.id << _file_queue.size();
                }

                found = true;
            }
        }
        return found;
    }

private:
    std::mutex _mutex;
    std::queue<FileID> _file_queue;
};

void processDocuments(QString dirname, QString output_dir, int max_size, int threads_number)
{
    FilesQueue queue;
    DictionaryBuilder dictionary;

    QDir dir(dirname);
    QStringList files = dir.entryList();
    for(int i=0; i<files.size(); ++i)
    {
        FileID fid;
        fid.path = dir.filePath(files[i]);
        QString name = files[i];

        QString id = name.mid(0, name.indexOf('.'));
        fid.id = id.toInt();
        queue.addFile(fid);
    }

    std::vector<std::thread> threads;

    for(int i=0; i<threads_number; ++i)
    {
        threads.push_back(std::thread([i, output_dir, max_size, &queue, &dictionary](){
            PairStorage storage(output_dir+"\\t"+QString::number(i)+"_", max_size, dictionary);
            FileID f;
            while(queue.getFileThreadSafe(f))
            {
                storage.readFile(f.path, f.id);
            }
        }));
    }

    for(std::thread& t : threads)
    {
        t.join();
    }
    dictionary.saveToFile(output_dir+"\\dictionary.txt");
}
