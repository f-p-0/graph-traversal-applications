/*
 * weighted_graph.cpp
 *
 * Method definitions for the weighted graph class.
 *
 * Author: Francisco Pineda
 */


#include "weighted_graph.h"

#include <iostream>
#include <limits>
#include <queue>
#include <vector>

using namespace std;



//struct used in the min heap during Dijkstra's algorithm
struct MinHeapNode {
  //vertex this node represents
  int vertex;

  //current best known distance to this vertex
  double currentDistance;

  MinHeapNode(int vertexVal, double currDistanceVal) {
    vertex = vertexVal;
    currentDistance = currDistanceVal;
  }

  bool operator>(const MinHeapNode& other) const {
    return currentDistance > other.currentDistance;
  }

};



WeightedGraph::WeightedGraph(int vertexCount) {
  _numVertices = vertexCount;
  _adjacencyList.resize(vertexCount);
}



void WeightedGraph::add_edge(int from, int to, double edgeWeight) {
  //add the "to" vertex id to the "from" vertex list, with corresponding weight
  _adjacencyList.at(from).emplace_back(to, edgeWeight);
}



vector<int> WeightedGraph::find_shortest_path(int from, int to) const {

  //if the from index equals the to index, return a vector with the vertex itself
  if (from == to) {
    return {from};
  }

  
  //declare min heap for Dijkstra's algorithm
  priority_queue<MinHeapNode, vector<MinHeapNode>, greater<MinHeapNode>> minHeap;

  
  //vector for storing predecessors vertices of a vertex (-1 = unknown predecessor vertex)
  vector<int> predecessors(_numVertices, -1);


  //vector for keeping track of best known distances to vertices
  //all distances except for the starting vertex distance begin at infinity
  vector<double> bestKnownDists(_numVertices, numeric_limits<double>::infinity());
  bestKnownDists.at(from) = 0.0;


  //vector to keep track of visited vertices
  vector<bool> visitedVertices(_numVertices, false);

  //add the starting vertex to the min heap
  minHeap.emplace(from, 0.0);

  
  //loop while the heap is not empty
  while (!(minHeap.empty())) {
    //get the top element from the heap and pop it
    MinHeapNode currElem = minHeap.top();
    minHeap.pop();

    //get the vertex the current min heap node associates with
    int currElemVertex = currElem.vertex;

    //if the "to" vertex was removed from the heap, the shortest path to it was found, break
    if (currElemVertex == to) {
      break;
    }

    //if the vertex has already been visited, this is a duplicate min heap node, ignore it
    if (visitedVertices.at(currElemVertex) == true) {
      continue;
    }

    //set the current vertex to be visited
    visitedVertices.at(currElemVertex) = true;


    //loop through all neighbors of current vertex
    for (size_t i = 0; i < _adjacencyList.at(currElemVertex).size(); i++) {
      //store the current "to" vertex being processed
      int currToVertex = _adjacencyList.at(currElemVertex).at(i).to;

      //if vertex is already visited, ignore it
      if (visitedVertices.at(currToVertex) == true) {
        continue;
      }


      //check if a shorter path to the current neighbor exists through the shortest distance vertex from the heap
      double foundDist = bestKnownDists.at(currElemVertex) + _adjacencyList.at(currElemVertex).at(i).weight;

      if (foundDist < bestKnownDists.at(currToVertex)) { //a shorter path found to the "to" vertex being processed

        //update the best known distances vector
        bestKnownDists.at(currToVertex) = foundDist;

        //insert new min heap node for the current "to" vertex with updated index
        minHeap.emplace(currToVertex, foundDist);

        //set the predecessor of the current "to" vertex to be the current vertex from the min heap
        predecessors.at(currToVertex) = currElemVertex;
      }

    }

  }


  //if the "to" vertex was never visited, the predecessor of "to" is still -1, and no path exists
  if (predecessors.at(to) == -1) {
    return {};
  }


  //build reverse path
  vector<int> reversePath;

  //start from the "to" vertex
  int currVertex = to;

  //loop until currVertex ends up on "from" vertex
  while (currVertex != from) {
    //add the current vertex to the reverse path
    reversePath.push_back(currVertex);

    //update the current vertex to be the element at that index in the predecessors vector
    currVertex = predecessors.at(currVertex);
  } 

  //add the last vertex, the "from" vertex, to the reverse path
  reversePath.push_back(from);


  //reverse the reverse path to get shortest path
  vector<int> path;
  for (size_t i = reversePath.size() - 1; i > 0; i--) {
    path.push_back(reversePath.at(i));
  }
  path.push_back(reversePath.at(0));


  //return the final path
  return path;


}



void WeightedGraph::print_all_edges() const {
  for (size_t i = 0; i < _adjacencyList.size(); i++) {
    cout << "edges for vertex " << i << ": ";
    if (_adjacencyList.at(i).size() != 0) {
      for (size_t j = 0; j < _adjacencyList.at(i).size() - 1; j++) {
        cout << _adjacencyList.at(i).at(j).to << " (weight: " << _adjacencyList.at(i).at(j).weight << "), ";
      } 
      cout << _adjacencyList.at(i).at(_adjacencyList.at(i).size() - 1).to << " (weight: " << _adjacencyList.at(i).at(_adjacencyList.at(i).size() - 1).weight << ")";
    }

    cout << endl << endl;
  }
}

