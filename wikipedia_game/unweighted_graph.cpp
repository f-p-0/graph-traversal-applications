/*
 * unweighted_graph.cpp
 *
 * Method definitions for the unweighted graph class.
 *
 * Author: Francisco Pineda
 */


#include "unweighted_graph.h"

#include <iostream>
#include <queue>
#include <vector>

using namespace std;



UnweightedGraph::UnweightedGraph(int vertexCount) {
  _numVertices = vertexCount;
  _adjacencyList.resize(vertexCount);
}



void UnweightedGraph::add_edge(int from, int to) {
  //add the "to" vertex id to the "from" vertex list
  _adjacencyList.at(from).push_back(to);
}



vector<int> UnweightedGraph::find_shortest_path(int from, int to) const {

  //if the from index equals the to index, return a vector with the vertex itself
  if (from == to) {
    return {from};
  }
  
  //initialize visited status of all vertices to false
  vector<bool> visited(_numVertices, false);

  //used in rebuilding path
  UnweightedGraph reverseGraph(_numVertices); 

  //queue for bfs
  queue<int> bfsQueue;

  //set the starting vertex as visited
  visited.at(from) = true;
  //add starting vertex to queue
  bfsQueue.push(from);

  //loop while queue is not empty and the destination (to) is not visited
  while ((!bfsQueue.empty()) && (visited.at(to) == false)) {
    //get vertex at the front of the queue
    int currFromVertex = bfsQueue.front();
    bfsQueue.pop();

    //loop through its edges in the adjacency list
    for (size_t i = 0; i < _adjacencyList.at(currFromVertex).size(); i++) {
      //current edge is the "to" vertex for the current "from" vertex
      int currToVertex = _adjacencyList.at(currFromVertex).at(i);
      
      //check if not visited
      if (!visited.at(currToVertex)) { 

        //add it to the queue
        bfsQueue.push(currToVertex);
        
        //mark as visited
        visited.at(currToVertex) = true;
        
        //add the reverse edge (to -> from) to the reverse graph
        reverseGraph.add_edge(currToVertex, currFromVertex);
      }
    }
  }

  //vector to store reverse path
  vector<int> reversePath;


  //no path found, return empty vector
  if (visited.at(to) == false) {
    return {};
  }

  
  //build reverse path
  reversePath.push_back(to);

  while (to != from) {
    //update "to" variable with the only vertex it has an edge to in reverse graph
    to = reverseGraph._adjacencyList.at(to).at(0);
    //add the vertex to the reverse path
    reversePath.push_back(to);
  }
  
  
  //reverse the reverse path to get shortest path
  vector<int> path;
  for (size_t i = reversePath.size() - 1; i > 0; i--) {
    path.push_back(reversePath.at(i));
  }
  path.push_back(reversePath.at(0));

  
  //return the final path
  return path;
}



void UnweightedGraph::print_all_edges() const {
  for (size_t i = 0; i < _adjacencyList.size(); i++) {
    cout << "edges for vertex " << i << ": ";
    if (_adjacencyList.at(i).size() != 0) {
      for (size_t j = 0; j < _adjacencyList.at(i).size() - 1; j++) {
        cout << _adjacencyList.at(i).at(j) << ", ";
      } 
      cout << _adjacencyList.at(i).at(_adjacencyList.at(i).size() - 1);
    }

    cout << endl << endl;
  }
}
