
#include <QtTest>
#include <QtCore/qnamespace.h>

class KGraphicsSignalPlotter;
#include <QGraphicsView>
#include <QGraphicsScene>
class BenchmarkGraphicsSignalPlotter : public QObject
{
    Q_OBJECT
    private slots:
        void init();
        void cleanup();

        void addData();
        void addDataWhenHidden();
    private:
        KGraphicsSignalPlotter *s;
        QGraphicsView *view;
        QGraphicsScene *scene;

};
