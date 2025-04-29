#include "frontend.h"
#include "backend.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QGraphicsView>
#include <QLabel>
#include <QTextEdit>
#include <QFileDialog>

HyperbolicityApp::HyperbolicityApp(QWidget *parent)
    : QMainWindow(parent), graph(nullptr)
{
    setWindowTitle("Hyperbolicity Calculator");
    resize(1200, 800);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QLabel *titleLabel = new QLabel("Hyperbolicity Calculator", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QHBoxLayout *topLayout = new QHBoxLayout();

    algorithmSelector = new QComboBox(this);
    algorithmSelector->addItem("Exact Algorithm");
    algorithmSelector->addItem("Factor-2 Approximation");
    algorithmSelector->addItem("Log Approximation");
    topLayout->addWidget(algorithmSelector);

    uploadButton = new QPushButton("Upload Graph File (CSV/JSON)", this);
    topLayout->addWidget(uploadButton);
    connect(uploadButton, &QPushButton::clicked, this, &HyperbolicityApp::uploadFile);

    computeButton = new QPushButton("Compute Hyperbolicity", this);
    topLayout->addWidget(computeButton);
    connect(computeButton, &QPushButton::clicked, this, &HyperbolicityApp::computeHyperbolicity);



    mainLayout->addLayout(topLayout);

    graphicsView = new QGraphicsView(this);
    scene = new QGraphicsScene(this);
    graphicsView->setScene(scene);
    graphicsView->setMinimumHeight(400);
    mainLayout->addWidget(graphicsView);

    resultLabel = new QLabel("Hyperbolicity: -", this);
    resultLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    mainLayout->addWidget(resultLabel);

    logText = new QTextEdit(this);
    logText->setReadOnly(true);
    mainLayout->addWidget(logText);
}

void HyperbolicityApp::uploadFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open Graph File", "",
                                                    "CSV Files (*.csv);;JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        logText->append(QString("✅ File Loaded: %1").arg(filePath));
        try {
            if (graph) delete graph;
            graph = loadGraph(filePath);
            currentGraphFilePath = filePath;
            
            QSize viewSize = graphicsView->size();
            QPixmap graphPixmap = drawGraph(graph, viewSize);
            scene->clear();
            scene->addPixmap(graphPixmap);
            graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
            logText->append("✅ Graph Loaded Successfully");
        } catch (std::exception &e) {
            logText->append(QString("❌ Error Loading Graph: %1").arg(e.what()));
        }
    }
}

void HyperbolicityApp::computeHyperbolicity() {
    if (!graph) {
        logText->append("⚠️ Please upload a graph first!");
        return;
    }
    try {
                QString method = algorithmSelector->currentText();
        int node_count = graph->nodes.size(); // Get node count

        logText->append("----------------------------------------");
        logText->append(QString("⏳ Starting computation using [%1] for graph with %2 nodes...")
                        .arg(method).arg(node_count));
        QApplication::processEvents(); // Allow UI to update

        // Call the backend function (which now includes timing)
        HyperResult result = ::computeHyperbolicity(graph, method, currentGraphFilePath);

        // Display hyperbolicity result
        resultLabel->setText(QString("Hyperbolicity: %1").arg(result.hyperbolicity));

        // *** LOG COMPLETION WITH TIME from HyperResult ***
        logText->append(QString("✅ Computation Complete using [%1]. Hyperbolicity: %2")
                        .arg(method).arg(result.hyperbolicity));
        logText->append(QString("   Nodes: %1. Total Time: %5 s")
                        .arg(result.node_order.size()) // Get n from result
                        .arg(QString::number(result.total_duration_ms/1000000.0, 'f', 7))); // Get duration from result
        logText->append("----------------------------------------");

    } catch (std::exception &e) {
        logText->append(QString("❌ Error Computing Hyperbolicity: %1").arg(e.what()));
    }
}



HyperbolicityApp::~HyperbolicityApp() {
    if (graph) delete graph;
}