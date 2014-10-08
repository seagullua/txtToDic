#include "Merger.h"
#include <QDebug>
FileReader::FileReader(QString file_name, bool read_header)
    : _file(file_name),
      _total(0),
      _readed(0),
      _stream(nullptr),
      _doc(1,1)
{
    if (_file.open(QIODevice::ReadOnly))
    {
        _stream = new QTextStream(&_file);

        if(read_header)
        {
            *_stream >> _total;
            qDebug() << _total;
        }


        pop();
    }
}
void FileReader::pop()
{
    _readed++;
    *_stream >> _doc.term >> _doc.docid;
}
FileReader::~FileReader()
{
    delete _stream;
}

Merger::Merger(QStringList files)
{
    for(int i=0; i<files.size(); ++i)
    {
        _readers.push_back(std::make_shared<FileReader>(files[i]));
    }
}

void Merger::saveToFile(QString output)
{
    QFile file(output);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);

        int processed = 0;
        while(_readers.size() > 0)
        {
            TermDoc min = _readers[0]->next();
            int min_i = 0;

            for(int i=0; i<_readers.size(); ++i)
            {
                FileReader& reader = *_readers[i];

                if(reader.next() < min)
                {
                    min = reader.next();
                    min_i = i;
                }

            }


            _readers[min_i]->pop();
            stream << min.term << ' ' << min.docid << endl;
            processed++;

            if(processed % 100000 == 0)
            {
                qDebug() << processed << min.term << _readers.size() << _readers[min_i]->_readed;
            }

            if(!_readers[min_i]->hasNext())
            {
                _readers.remove(min_i);
            }

        }

    }
}

void optimise(QString input, QString output)
{
    FileReader reader(input);
    QFile file(output);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);

        int last_word = 0;

        int processed = 0;
        while(reader.hasNext())
        {
            TermDoc current = reader.next();
            reader.pop();
            std::swap(current.docid, current.term);

            if(last_word != current.term)
            {
                last_word = current.term;
                stream << endl << last_word;
            }

            stream << ' ' << current.docid;

            processed++;
            if(processed % 100000 == 0)
            {
                qDebug() << processed << last_word;
            }
        }
    }
}
