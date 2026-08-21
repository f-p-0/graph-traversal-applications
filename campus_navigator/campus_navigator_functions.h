/*
 * campus_navigator_functions.h
 *
 * Declaration of functions used by campus navigator.
 * 
 * Author: Francisco Pineda
 */

#ifndef CAMPUS_NAVIGATOR_FUNCTIONS_H
#define CAMPUS_NAVIGATOR_FUNCTIONS_H

#include "map_node.h"
#include "weighted_graph.h"

#include <string>
#include <unordered_map>
#include <vector>



/**
 * @brief set the circle properties while reading vertex data
 * 
 * @param mapNodesCounter index of map node being modified
 * @param mapNodes vector of map nodes
 * @param buildingRadius radius given to building circles as file is read
 * @param intersectionRadius radius given to intersection circles as file is read
 * @param xPos x position of circle
 * @param yPos y position of circle
 */
void set_circle_properties(const int mapNodesCounter, std::vector<MapNode>& mapNodes, const float buildingRadius, const float intersectionRadius, const int xPos, const int yPos);
/**
 * @brief extract and process a line from the vertex data csv
 * 
 * @param mapNodes vector of map nodes
 * @param mapNodesCounter counter for which map node is being modified
 * @param vertexDataLine line from vertex data being processed
 * @param nameToID map from node names to an integer id
 * @param buildingRadius radius given to building circles as file is read
 * @param intersectionRadius radius given to intersection circles as file is read
 */
void extract_vertex_file_line(std::vector<MapNode>& mapNodes, int& mapNodesCounter, const std::string& vertexDataLine, std::unordered_map<std::string, int>& nameToID, const float buildingRadius, const float intersectionRadius);
/**
 * @brief read in the vertex data csv for the campus map
 * 
 * @param mapNodes vector of map nodes
 * @param nameToID map from names of locations to an integer id
 * @param buildingRadius radius given to building circles as file is read
 * @param intersectionRadius radius given to intersection circles as file is read
 * @return true if vertex file read successfully
 * @return false if failed to open vertex data file 
 */
bool read_vertex_data(std::vector<MapNode>& mapNodes, std::unordered_map<std::string, int>& nameToID, const float buildingRadius, const float intersectionRadius);
/**
 * @brief get the distance between two CircleShape objects
 * 
 * @param firstNode first CircleShape object
 * @param secondNode second CircleShape object
 * @return double Euclidean distance between circles 
 */
double get_node_distance(const sf::CircleShape& firstNode, const sf::CircleShape& secondNode);
/**
 * @brief adds an edge to the weighted graph, representing a path between two map nodes
 * 
 * @param campusGraph graph object to add edge to
 * @param firstVertexName name of first node location
 * @param secondVertexName name of second node location
 * @param nameToID map from names of locations to an integer id
 * @param mapNodes vector of map node structs
 */
void add_map_edges(WeightedGraph& campusGraph, const std::string& firstVertexName, const std::string& secondVertexName, const std::unordered_map<std::string, int>& nameToID, const std::vector<MapNode>& mapNodes);
/**
 * @brief extract and process a line from the edge data csv
 * 
 * @param campusGraph graph representing campus map
 * @param nameToID map from names of locations to an integer id
 * @param mapNodes vector of map nodes
 * @param edgeDataLine line from edge data csv being processed
 */
void extract_edge_data_line(WeightedGraph& campusGraph, const std::unordered_map<std::string, int>& nameToID, const std::vector<MapNode>& mapNodes, const std::string& edgeDataLine);
/**
 * @brief read the edge data csv for the campus map
 * 
 * @param campusGraph graph representing campus map
 * @param nameToID map from names of locations to an integer id
 * @param mapNodes vector of map nodes
 * @return true if edge file read successfully
 * @return false if failed to open edge file
 */
bool read_edge_data(WeightedGraph& campusGraph, const std::unordered_map<std::string, int>& nameToID, const std::vector<MapNode>& mapNodes);



#endif