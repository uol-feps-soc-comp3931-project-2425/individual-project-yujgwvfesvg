#ifndef FRONTEND_H
#define FRONTEND_H

#include <QMainWindow>

class QComboBox;
class QPushButton;
class QGraphicsView;
class QGraphicsScene;
class QLabel;
class QTextEdit;
class Graph;  // forward declaration from backend

class HyperbolicityApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit HyperbolicityApp(QWidget *parent = nullptr);
    ~HyperbolicityApp();

private slots:
    void uploadFile();
    void computeHyperbolicity();

private:
    QComboBox *algorithmSelector;
    QPushButton *uploadButton;
    QPushButton *computeButton;
    QPushButton *exportButton;
    QGraphicsView *graphicsView;
    QGraphicsScene *scene;
    QLabel *resultLabel;
    QTextEdit *logText;

    // Graph pointer from the backend.
    Graph* graph;
    QString currentGraphFilePath;
};

#endif // FRONTEND_H
