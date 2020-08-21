#ifndef LARGEFILE_H
#define LARGEFILE_H

/*
Класс для чтения и обработки файлов формата "*.txt"
Одинночные буквы и символы пунктуации игнорируются.
*/
#include <QObject>
#include <QRunnable>
#include <QFile>
#include <QPair>
#include <QChart>

#include <QMessageBox>
#include <QtDebug>
#include <QTextCodec>
#include <QThreadPool>


#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>

#include <QFuture>
#include <QtConcurrent>

#include <qtconcurrentmap.h>

static const quint64 DEFAULT_SIZE = 10000;
static const quint64 MIN_SIZE = 1000;

typedef QMap<QString, int> WordCount;
typedef std::pair<QString, int> WordPair;

#include <QMap>
#include <QString>
#include <QStringList>

#include <vector>

QT_CHARTS_USE_NAMESPACE

using namespace std;

class LargeFile : public QObject,
        public QRunnable
{
    Q_OBJECT
public:
    enum ResultCode { // статусы возвращаемые из потока
        RESULT_OK,
        RESULT_FAILED,
        RESULT_CANCELLED,
        RESULT_LOADING
    };

    LargeFile( const QString& fileName, quint64 chunkSizeKb = DEFAULT_SIZE ); // 
    ~LargeFile();

    void run();

public slots:
    void cancel();  // пользователь нажал кнопку "Отмена"

signals:
    void progressChanged( int doneCount, int sumCount); // прогресс чтения и обработки данных
    void readingFinished( int resultCode); 
    void creatingChart( QBarSet* set, const QStringList& categories); // 

private:
    void create_histogram(vector<WordPair>& result); // создание гистограммы
    void create_result_List(const WordCount& countWords, vector<WordPair>& result); // сохраняем топ - 15, сортировка по алфавиту
    void createData(const WordCount &result);

    QFile m_file;
    quint64 m_chunkSizeKb;
    QAtomicInt m_cancelledMarker;
    QMap<QString, int> m_worldList;

};

#endif // LARGEFILE_H
