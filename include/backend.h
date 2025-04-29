#ifndef BACKEND_H
#define BACKEND_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QSet>
#include <QPixmap>
#include <QSize>


//temp struct to print the shortest distance matrix
struct HyperResult {
    double hyperbolicity;
    std::vector<std::vector<double>> distance_matrix;
    QVector<QString> node_order;  // Use Qt container for compatibility
    long long total_duration_ms = 0; 


};

// Simple structure to hold an edge.
struct Edge {
    QString source;
    QString target;
    double weight;
};


class Graph {
public:
    QList<QString> nodes;  // Ordered node list
    QVector<Edge> edges;
    QMap<QString, QMap<QString, double>> adj;
    void addEdge(const QString& src, const QString& tgt, double weight);
};

// Functions implemented in backend.cpp.
Graph* loadGraph(const QString& filePath);
// double computeHyperbolicity(const Graph* graph, const QString& method);
HyperResult computeHyperbolicity(const Graph* graph, const QString& method, const QString& input_filename);
QPixmap drawGraph(const Graph* graph, const QSize& size);

#endif // BACKEND_H
