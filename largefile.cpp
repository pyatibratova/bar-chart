#include "largefile.h"

LargeFile::LargeFile( const QString& fileName, quint64 chunkSizeKb ) :
    QObject( nullptr ), m_file( fileName ), m_chunkSizeKb( chunkSizeKb ), m_cancelledMarker( false )
{

    if( m_chunkSizeKb < MIN_SIZE ) {
        m_chunkSizeKb = MIN_SIZE;
    }
}

LargeFile::~LargeFile()
{
}

void LargeFile::run() {
    if( !m_file.open( QIODevice::ReadOnly | QIODevice::Text) ) {
        emit readingFinished( RESULT_FAILED );
        return;
    }

    QTextStream textStream(&m_file);
    size_t data_size{};
    WordCount allCount{};
    QByteArray chunk{};
    do{
        if( m_cancelledMarker.testAndSetAcquire( true, true ) ) { // нажали отмену
            emit readingFinished( RESULT_CANCELLED );
            return;
        }
        try {
            chunk = m_file.read(m_chunkSizeKb * 1024 ); // читаем файл блоками
            QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
            QString str = codec->toUnicode(chunk);
            auto words = str.split(QRegExp("\\W+"), QString::SkipEmptyParts);
             for( const auto &word : words) {
                if( word.length() > 1) {
                     ++allCount[word];
                }
            }
            data_size += chunk.size();

        }  catch( ... ) {
            emit readingFinished( RESULT_FAILED );
            return;
        }
        if( m_file.error() != QFile::NoError ) {
            emit readingFinished( RESULT_FAILED );
            return;
        }

        emit progressChanged( data_size, m_file.size());
        createData(allCount);
    } while( !chunk.isEmpty() );

    emit progressChanged( m_file.size(), m_file.size());
    emit readingFinished( RESULT_OK);
}


void LargeFile::cancel()
{
    m_cancelledMarker.fetchAndStoreAcquire( true );
}

void LargeFile::createData(const WordCount& allCount)
{
   vector<WordPair> res{};
   create_result_List(allCount, res);
   create_histogram(res);
}

void LargeFile::create_result_List(const WordCount& countWords, vector<WordPair> &result)
{
    if( countWords.empty() ) {
        emit readingFinished( RESULT_FAILED );
        return;
    }

    // сохраняем в вектор
    QVector<pair<QString, int>> word_counts;
    word_counts.reserve(countWords.size());
  QMapIterator<QString, int> i(countWords);
    while (i.hasNext()) {
        i.next();
        pair<QString, int> value(i.key(), i.value());
        word_counts.append(value);
    }

    // сортируем по частоте
    sort(begin(word_counts), end(word_counts),
         [](const pair<QString, int> &a, const pair<QString, int>& b) {
        return a.second >  b.second;
    });

    // надо вывести 15 слов, если меньше, то выводим все
    auto count = ( word_counts.size() >= 15 ) ? 15 : word_counts.size();
    copy(begin(word_counts), begin(word_counts) + count, back_inserter(result));

    // сортируем по алфавиту
    sort(begin(result), end(result),
         [](const pair<QString, int> &a, const pair<QString, int>& b) {
        return a.first.toLower() <  b.first.toLower();
    });


}

// подготовка данных к выводу на экран
void  LargeFile::create_histogram(vector<WordPair> &result)
{
    QBarSet *set = new QBarSet("");
    QStringList categories{};
    int j{0};
    for( const auto& i : result ) {
        j++;
        categories << i.first;
        *set << i.second;
    }
    emit creatingChart(set, categories);
}
