#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

/*
Построить гистограмму на top-15(по количество вхождений) слов в выбранном
файле произвольного размера.
После выбора файла, должен быть виден прогресс обработки файла.
Хорошим бонусом станет обновление гистограммы в условно реальном
времени(без видимых продолжительных задержек).
Топ 15 слов, должны быть, отсортированы по алфавиту.
Подсчет слов и построение UI должны быть разделены минимум на два разных
потока.
*/
#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>

#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>

QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
class Ui_ChartWidgetForm;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE
QT_CHARTS_USE_NAMESPACE

class ChartWidget: public QWidget
{
    Q_OBJECT
public:
    explicit ChartWidget(QWidget *parent = nullptr);
    ~ChartWidget();

signals:
    void cancelTaskRequired();
public slots:
    void openFile();
    void setProgress(const int &doneCount, const int &sumCount );
    void readingFinished(int resultCode);
    void creatingChart(QBarSet *set, const QStringList& categories);
    void cancel();

private:
    QChartView* m_chartView;
    Ui_ChartWidgetForm *m_ui;

    void clearChartView();
};

#endif
