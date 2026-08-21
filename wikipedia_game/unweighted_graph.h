/*
 * unweighted_graph.h
 *
 * Declaration of the unweighted graph class.
 * 
 * Author: Francisco Pineda
 */

#ifndef UNWEIGHTED_GRAPH_H
#define UNWEIGHTED_GRAPH_H

#include <vector>



class UnweightedGraph {
public:
  /**
   * @brief construct a new Unweighted Graph object
   * 
   * @param vertexCount number of vertices in graph
   */
  UnweightedGraph(int vertexCount);
  /**
   * @brief add an edge in the graph between two vertices
   * 
   * @param from source vertex of edge
   * @param to target vertex of edge
   */
  void add_edge(int from, int to);
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
   * @brief number of vertices stored in graph
   * 
   */
  int _numVertices;
  /**
   * @brief adjacency list representation of graph
   * 
   */
  std::vector<std::vector<int>> _adjacencyList; 

};



#endif