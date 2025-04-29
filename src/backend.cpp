#include "backend.h"
#include "GraphParser.h"
#include "ShortestPath.h"
#include "Hyperbolicity.h"
#include "2Factor.h"
#include "LogApproximation.h"
#include <QFile>
#include <QJsonDocument>
#include <QPainter>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QFileInfo>
#include <QDir>  
#include <iostream>
#include <QCoreApplication>
#include <limits>
#include <QDebug>


constexpr double INF = std::numeric_limits<double>::infinity();

void Graph::addEdge(const QString& src, const QString& tgt, double weight) {
    // nodes.insert(src);
    // nodes.insert(tgt);
    edges.append({src, tgt, weight});
    adj[src][tgt] = weight;
    adj[tgt][src] = weight;
}

Graph* loadGraph(const QString& filePath) {
    Graph* graph = new Graph();
    QFile file(filePath);
  
     if (filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        GraphParser parser;
        parser.parse(filePath.toStdString());
        
        // Preserve node order from parser
        const auto& std_nodes = parser.get_nodes();
        for (const auto& node : std_nodes) {
            QString qNode = QString::fromStdString(node);
            graph->nodes.append(qNode); // Maintain order using QList
        }
        
        // Add edges (existing code)
        const auto& adj_list = parser.get_adjacency_list();
        for (size_t i = 0; i < adj_list.size(); ++i) {
            QString src = QString::fromStdString(std_nodes[i]);
            for (const auto& [j, weight] : adj_list[i]) {
                QString dest = QString::fromStdString(std_nodes[j]);
                graph->addEdge(src, dest, weight);
            }
        }
    } 
    else if (filePath.endsWith(".json", Qt::CaseInsensitive)) {
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Cannot open file");
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) throw std::runtime_error("Invalid JSON format");
    
    QJsonObject root = doc.object();
    QJsonArray edges = root.value(QLatin1String("edges")).toArray();
    
    for (const QJsonValue& edge_val : edges) {
        QJsonObject edge = edge_val.toObject();
        QString src = edge.value(QLatin1String("source")).toString();
        QString dest = edge.value(QLatin1String("target")).toString();
        double weight = edge.contains(QLatin1String("weight")) 
                      ? edge.value(QLatin1String("weight")).toDouble() 
                      : 1.0;
        graph->addEdge(src, dest, weight);
    }
} else {
        throw std::runtime_error("Unsupported file format");
    }
    return graph;
}

HyperResult computeHyperbolicity(const Graph* graph, const QString& method, const QString& input_filename) {
    HyperResult result;
    auto overall_start_time = std::chrono::steady_clock::now();

    
    // Capture node order
    result.node_order = graph->nodes.toVector();
    
    // Create index mapping
    QMap<QString, int> node_indices;
    for (int i = 0; i < graph->nodes.size(); ++i) {
        node_indices[graph->nodes[i]] = i;
    }
    
    // Build adjacency list
    std::vector<std::vector<std::pair<int, double>>> adj_list(graph->nodes.size());
    for (const Edge& edge : graph->edges) {
        int u = node_indices[edge.source];
        int v = node_indices[edge.target];
        adj_list[u].emplace_back(v, edge.weight);
    }
    //distance matrix
    std::vector<std::vector<double>> distance_matrix; // Use local variable first
    
    // Compute distances

    ShortestPath sp(adj_list);
    distance_matrix = sp.compute_all_pairs(); 
    result.distance_matrix = distance_matrix;
    

    
    // Compute hyperbolicity
    if (method == "Exact Algorithm") {
        HyperbolicityCalculator exact_calc(result.distance_matrix);
        double delta_r = exact_calc.compute_exact_hyperbolicity();
        result.hyperbolicity = delta_r;//delta_r;

    } else if (method == "Factor-2 Approximation") {
        TwoFactorApproximation approx_calc(result.distance_matrix);
        result.hyperbolicity = approx_calc.compute_approximate_hyperbolicity();
    } else if (method == "Log Approximation") {
        LogApproxHyperbolicity log_approx(result.distance_matrix, adj_list);
        result.hyperbolicity = log_approx.compute_approximate_hyperbolicity();
    }
    // --- End Hyperbolicity Algorithm Computation ---


    // --- Calculate and store TOTAL duration in MICROSECONDS ---
    auto overall_end_time = std::chrono::steady_clock::now(); // Use steady_clock
    auto overall_duration = std::chrono::duration_cast<std::chrono::microseconds>(overall_end_time - overall_start_time);
    result.total_duration_ms = overall_duration.count(); // Store us
    

    return result;
}




QPixmap drawGraph(const Graph* graph, const QSize& size) {
    QPixmap pixmap(size);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // QList<QString> nodeList = graph->nodes.values();
    QList<QString> nodeList = graph->nodes; 
    std::sort(nodeList.begin(), nodeList.end());
    if (nodeList.size() > 50) nodeList = nodeList.mid(0, 50);

    const int n = nodeList.size();
    const QPoint center(size.width()/2, size.height()/2);
    const int radius = std::min(size.width(), size.height())/2 - 50;
    QMap<QString, QPoint> positions;

    for (int i = 0; i < n; ++i) {
        double angle = 2 * M_PI * i / n;
        positions[nodeList[i]] = QPoint(
            center.x() + static_cast<int>(radius * cos(angle)),
            center.y() + static_cast<int>(radius * sin(angle))
        );
    }

    painter.setPen(QPen(Qt::gray, 2));
    for (const Edge& edge : graph->edges) {
        if (positions.contains(edge.source) && positions.contains(edge.target)) {
            painter.drawLine(positions[edge.source], positions[edge.target]);
        }
    }
//-------------------
    // Draw edges with weights
    painter.setPen(QPen(Qt::gray, 2));
    QFont weightFont = painter.font();
    weightFont.setPointSize(10);
    painter.setFont(weightFont);

    for (const Edge& edge : graph->edges) {
        if (positions.contains(edge.source) && positions.contains(edge.target)) {
            QPoint p1 = positions[edge.source];
            QPoint p2 = positions[edge.target];
            
            // Draw edge line
            painter.drawLine(p1, p2);

            // Only draw weight once per edge pair (A-B and B-A)
            if (nodeList.indexOf(edge.source) < nodeList.indexOf(edge.target)) {
                // Calculate midpoint with offset
                QPoint midpoint = (p1 + p2) / 2;
                QPoint textOffset(10, -5);
                
                // Draw weight background
                painter.setBrush(QColor(255, 255, 255, 200));  // Semi-transparent white
                painter.setPen(Qt::NoPen);
                QRect textRect = painter.boundingRect(QRect(), Qt::AlignCenter, 
                    QString::number(edge.weight, 'f', 2));
                textRect.moveTo(midpoint + textOffset);
                painter.drawRect(textRect);
                
                // Draw weight text
                painter.setPen(Qt::darkBlue);
                painter.drawText(midpoint + textOffset, 
                    QString::number(edge.weight, 'f', 2));
                
                // Reset pen for next edge
                painter.setPen(QPen(Qt::gray, 2));
            }
        }
    }
//-----------------------------

    const int nodeSize = 20;
    painter.setBrush(Qt::lightGray);
    painter.setPen(Qt::black);
    for (auto it = positions.begin(); it != positions.end(); ++it) {
        QPoint pos = it.value();
        painter.drawEllipse(pos, nodeSize, nodeSize);
        painter.drawText(QRect(pos.x()-nodeSize, pos.y()-nodeSize, 
                            2*nodeSize, 2*nodeSize), Qt::AlignCenter, it.key());
    }

    return pixmap;
}