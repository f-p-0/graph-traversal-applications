/*
 * weighted_graph.h
 *
 * Declaration of the weighted graph class.
 * 
 * Author: Francisco Pineda
 */

#ifndef WEIGHTED_GRAPH_H
#define WEIGHTED_GRAPH_H

#include <vector>



class WeightedGraph {
public:
  /**
   * @brief construct a new Weighted Graph object
   * 
   * @param vertexCount number of vertices in graph
   */
  WeightedGraph(int vertexCount);
  /**
   * @brief add an edge in the graph between two vertices
   * 
   * @param from source vertex of edge
   * @param to target vertex of edge
   * @param edgeWeight weight of edge (default value of 1)
   */
  void add_edge(int from, int to, double edgeWeight = 1.0);
  /**
   * @brief find the shortest path between two vertices
   * 
   * @param from source vertex
   * @param to target vertex
   * @return std::vector<int> vector with vertices along shortest path 
   */
  std::vector<int> find_shortest_path(int from, int to) const;
  /**
   * @brief prints all edges of graph (used in testing)
   * 
   */
  void print_all_edges() const;

private:
  /**
   * @brief weighted edge struct, stores "to" vertex and edge weight
   * 
   */
  struct WeightedEdge {
    int to;
    double weight;

    WeightedEdge(int toField, double weightField) {
      to = toField;
      weight = weightField;
    }
  };
  /**
   * @brief number of vertices stored in graph
   * 
   */
  int _numVertices;
  /**
   * @brief adjacency list representation of graph
   * 
   */
  std::vector<std::vector<WeightedEdge>> _adjacencyList;

};



#endif