/*
 * campus_navigator_functions.cpp
 *
 * Definitions for the functions used by campus navigator.
 *
 * Author: Francisco Pineda
 */


#include "campus_navigator_functions.h"
#include "map_node.h"
#include "weighted_graph.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>

using namespace sf;
using namespace std;



void set_circle_properties(const int mapNodesCounter, vector<MapNode>& mapNodes, const float buildingRadius, const float intersectionRadius, const int xPos, const int yPos) {
  //based on the character, set up the color and radius fields of the circle

  //get the type of the current node
  char currNodeType = mapNodes.at(mapNodesCounter).type;

  //set the radius and color of the circle
  float nodeRadius;
  Color nodeColor;
  if (currNodeType == 'b') { //building
    nodeRadius = buildingRadius;
    nodeColor = Color(Color::Yellow.r, Color::Yellow.g, Color::Yellow.b, 150); //building color more transparent so names can be seen
  } else { //intersection
    nodeRadius = intersectionRadius;
    nodeColor = Color::Black;
  }

  
  //assign the properties of the circle for the current node
  mapNodes.at(mapNodesCounter).nodeCircle.setPosition(Vector2f(xPos, yPos));
  mapNodes.at(mapNodesCounter).nodeCircle.setRadius(nodeRadius);
  mapNodes.at(mapNodesCounter).nodeCircle.setFillColor(nodeColor);
  mapNodes.at(mapNodesCounter).nodeCircle.setOrigin(nodeRadius, nodeRadius);
}



void extract_vertex_file_line(vector<MapNode>& mapNodes, int& mapNodesCounter, const string& vertexDataLine, unordered_map<string, int>& nameToID, const float buildingRadius, const float intersectionRadius) {
  //counter for keeping track of location within line
  size_t lineCounter = 0;

  //string to store x position of vertex
  string xPosStr = "";

  //loop until first comma
  while (vertexDataLine.at(lineCounter) != ',') {
    xPosStr += vertexDataLine.at(lineCounter);
    lineCounter++;
  }

  //convert x position to a float
  float xPos = stof(xPosStr);


  //advance counter one more to move past comma, ready for next field
  lineCounter++;

  //string to store y position of vertex
  string yPosStr = "";

  //loop until next comma
  while (vertexDataLine.at(lineCounter) != ',') {
    yPosStr += vertexDataLine.at(lineCounter);
    lineCounter++;
  }

  //convert y position to float
  float yPos = stof(yPosStr);
  
  //advance counter one more to move past comma, ready for next field
  lineCounter++;


  //counter at character field, read and assign to map node
  char currNodeType = vertexDataLine.at(lineCounter);
  mapNodes.at(mapNodesCounter).type = currNodeType;


  //set the properties of the circle object that will later be drawn
  set_circle_properties(mapNodesCounter, mapNodes, buildingRadius, intersectionRadius, xPos, yPos);


  //advance counter twice more to move past character and comma, ready for next field
  lineCounter += 2;


  //counter is at start of name field, 2 cases

  string nameField = "";

  //case 1: name field is enclosed in set of double quotes
  if (vertexDataLine.at(lineCounter) == '\"') {
    //move counter past first quote
    lineCounter++;

    //read until next quote
    while (vertexDataLine.at(lineCounter) != '\"') {
      nameField += vertexDataLine.at(lineCounter);
      lineCounter++;
    }

  //case 2: name field is not enclosed in a set of double quotes
  } else {
    //counter already at first character in name field, read until end of line
    while (lineCounter < vertexDataLine.size()) {
      nameField += vertexDataLine.at(lineCounter);
      lineCounter++;
    }
  }

  //set the map node field to be the name extracted
  mapNodes.at(mapNodesCounter).name = nameField;

  //add the pair (name, id) into the map, where the id is the map nodes counter
  nameToID.emplace(nameField, mapNodesCounter);
  
  //increment the map nodes counter
  mapNodesCounter++;
}



bool read_vertex_data(vector<MapNode>& mapNodes, unordered_map<string, int>& nameToID, const float buildingRadius, const float intersectionRadius) {
  //open custom dataset containing information about node locations
  ifstream vertexData("./map_data/vertex_data.csv");

  //check if file opened successfully, return false if it didn't
  if (vertexData.fail()) {
    cerr << "Error: failed to open vertex data file" << endl;
    return false;
  }

  //string to hold line from file
  string vertexDataLine;

  //read in first line that only contains csv info
  getline(vertexData, vertexDataLine);


  //counter through map nodes vector for setting fields of objects
  int mapNodesCounter = 0;


  //read vertex_data.csv, building map nodes
  while (getline(vertexData, vertexDataLine)) {
    //extract and process the line from the vertex data
    extract_vertex_file_line(mapNodes, mapNodesCounter, vertexDataLine, nameToID, buildingRadius, intersectionRadius);

  }

  //return true for file being read successfully
  return true;
}



double get_node_distance(const CircleShape& firstNode, const CircleShape& secondNode) {
  //get the x and y coordinates for the nodes 
  Vector2f firstNodePos = firstNode.getPosition();
  Vector2f secondNodePos = secondNode.getPosition();

  //calculate the distance and return it
  double distance = hypot(firstNodePos.x - secondNodePos.x, firstNodePos.y - secondNodePos.y);
  return distance;

}



void add_map_edges(WeightedGraph& campusGraph, const string& firstVertexName, const string& secondVertexName, const unordered_map<string, int>& nameToID, const vector<MapNode>& mapNodes) {
  //look for the vertex names in the map
  auto firstVertexIt = nameToID.find(firstVertexName);
  auto secondVertexIt = nameToID.find(secondVertexName);

  //check that both iterators exist before attempting to make an edge
  if (firstVertexIt == nameToID.end() || secondVertexIt == nameToID.end()) {
    cerr << "Error: Cannot make an edge between " << firstVertexName << " and " << secondVertexName << endl;
    return;
  }

  //convert the first and second vertex names to their integer id's
  int firstVertexID = firstVertexIt->second;
  int secondVertexID = secondVertexIt->second;
  
  //find the distance between the nodes
  double distance = get_node_distance(mapNodes.at(firstVertexID).nodeCircle, mapNodes.at(secondVertexID).nodeCircle);

  //add the edges (first, second) and (second, first) to the graph object (the graph for this application is an undirected graph)
  campusGraph.add_edge(firstVertexID, secondVertexID, distance);
  campusGraph.add_edge(secondVertexID, firstVertexID, distance);
  
}



void extract_edge_data_line(WeightedGraph& campusGraph, const unordered_map<string, int>& nameToID, const vector<MapNode>& mapNodes, const string& edgeDataLine) {
  //counter for keeping track of location within line
  size_t lineCounter = 0;

  //string to hold first vertex name
  string firstVertexName = "";

  //counter is at start of name field, 2 cases

  //case 1: first vertex name field is enclosed in set of double quotes
  if (edgeDataLine.at(lineCounter) == '\"') {
    //move counter past first quote
    lineCounter++;

    //read until next quote
    while (edgeDataLine.at(lineCounter) != '\"') {
      firstVertexName += edgeDataLine.at(lineCounter);
      lineCounter++;
    }

  //case 2: first vertex name field is not enclosed in set of double quotes
  } else {
    //counter already at first character in name field, read until next comma
    while (edgeDataLine.at(lineCounter) != ',') {
      firstVertexName += edgeDataLine.at(lineCounter);
      lineCounter++;
    }
    
  }


  //if name had a quote, line counter is on a quote, and should be skipped
  if (edgeDataLine.at(lineCounter) == '\"') {
    lineCounter++;
  }

  //increment counter past comma in either case
  lineCounter++;


  //string to hold second vertex name
  string secondVertexName = "";


  //counter is at start of name field, 2 cases

  //case 1: second vertex name field is enclosed in set of double quotes
  if (edgeDataLine.at(lineCounter) == '\"') {
    //move counter past first quote
    lineCounter++;

    //read until next quote
    while (edgeDataLine.at(lineCounter) != '\"') {
      secondVertexName += edgeDataLine.at(lineCounter);
      lineCounter++;
    }

  //case 2: second vertex name field is not enclosed in set of double quotes
  } else {
    //counter already at first character in name field, read until end of line
    while (lineCounter < edgeDataLine.size()) {
      secondVertexName += edgeDataLine.at(lineCounter);
      lineCounter++;
    }
  }


  //set up edges between the vertices
  add_map_edges(campusGraph, firstVertexName, secondVertexName, nameToID, mapNodes);
}



bool read_edge_data(WeightedGraph& campusGraph, const unordered_map<string, int>& nameToID, const vector<MapNode>& mapNodes) {
  //open custom dataset containing information about edges between nodes
  ifstream edgeData("./map_data/edge_data.csv");

  //check if file opened successfully, return false if it didn't
  if (edgeData.fail()) {
    cerr << "Error: failed to open edge data file" << endl;
    return false;
  }

  //string to hold line from file
  string edgeDataLine;

  //read in first line that only contains csv info
  getline(edgeData, edgeDataLine);
  

  //read edge_data.csv, building graph object
  while (getline(edgeData, edgeDataLine)) {
    //extract and process the line from the edge data
    extract_edge_data_line(campusGraph, nameToID, mapNodes, edgeDataLine);
  }

  //return true for file being read successfully
  return true;
  
}
