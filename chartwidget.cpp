
#include "chartwidget.h"
#include "ui_chartwidget.h"

#include <QtCharts/QChartView>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>

#include <QtCore/QRandomGenerator>
#include <QtCharts/QBarCategoryAxis>
#include <QtWidgets/QApplication>
#include <QtCharts/QValueAxis>

#include <QStackedBarSeries>
#include <QFileDialog>
#include <QThreadPool>

#include "largefile.h"

ChartWidget::ChartWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui_ChartWidgetForm)
{
    m_ui->setupUi(this);
    m_chartView = new QChartView();
    m_ui->gridLayout->addWidget(m_chartView);

    QPalette pal = qApp->palette();
    pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
    pal.setColor(QPalette::WindowText, QRgb(0x404044));
    qApp->setPalette(pal);

    QObject::connect(m_ui->openButton, &QPushButton::clicked, this, &ChartWidget::openFile);
}

ChartWidget::~ChartWidget()
{
    delete m_ui;
}

// обработка событий по нажатию кнопки "Открыть"
// чтение и обработка данных производиться в отдельном потоке
void ChartWidget::openFile()
{
    auto fileName = QFileDialog::getOpenFileName( this, "Open some large file" );
    if( fileName.isEmpty() ) {
        m_ui->label->setText("Eror: file name is empty");
        return;
    }

    LargeFile* task = new LargeFile( fileName );
    connect( task, &LargeFile::progressChanged, this, &ChartWidget::setProgress);
    connect( task, &LargeFile::readingFinished, this, &ChartWidget::readingFinished);
    connect(task, &LargeFile::creatingChart, this, &ChartWidget::creatingChart);
    connect(m_ui->cancelButton, &QPushButton::clicked, task, &LargeFile::cancel);
    connect( this, &ChartWidget::cancelTaskRequired, task, &LargeFile::cancel);

    QThreadPool::globalInstance()->start( task );
}

// прогресс чтения и обработки файла
void ChartWidget::setProgress(const int &doneCount, const int &sumCount)
{
    m_ui->progressBar->setMaximum( sumCount );
    m_ui->progressBar->setValue( doneCount );
}

// статус обработки файла
void ChartWidget::readingFinished(int resultCode)
{
    switch ( resultCode ) {
    case LargeFile::RESULT_OK:
        m_ui->label->setText("Ok. Read file");
        break;
    case LargeFile ::RESULT_FAILED:
        m_ui->label->setText("Error: reading file");
        break;
    case LargeFile::RESULT_CANCELLED:
        m_ui->label->setText("Cancelled");
        cancel();
        break;
    }
}

// создаем гистограммы
void ChartWidget::creatingChart(QBarSet *set, const QStringList &categories)
{
    clearChartView();
    m_chartView->chart()->createDefaultAxes();
    auto *series = new QBarSeries();
    series->append(set);
    m_chartView->chart()->addSeries(series);
    m_chartView->chart()->setTitle("top - 15");
    m_chartView->chart()->setAnimationOptions(QChart::SeriesAnimations);

    auto *axisX = new QBarCategoryAxis();
    axisX->categories().clear();
    axisX->append(categories);
    m_chartView->chart()->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    m_chartView->chart()->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}

// обработка событий по нажатию кнопки "Отмена"
void ChartWidget::cancel()
{
    m_ui->progressBar->setValue(0);
    clearChartView();
}

// удаляем данные
void ChartWidget::clearChartView()
{
    m_ui->label->clear();
    m_chartView->chart()->title().clear();
    m_chartView->chart()->removeAllSeries();
    m_chartView->chart()->removeAxis(m_chartView->chart()->axisX());
    m_chartView->chart()->removeAxis(m_chartView->chart()->axisY());
    m_chartView->update();
}
