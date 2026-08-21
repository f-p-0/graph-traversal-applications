/*
 * map_node.h
 *
 * Declaration of map node used for campus navigator.
 *
 * Author: Francisco Pineda
 */

#ifndef MAP_NODE_H
#define MAP_NODE_H

#include <SFML/Graphics.hpp>

#include <string>



/**
 * @brief struct for storing info about the nodes in the map
 * 
 */
struct MapNode {
  /**
   * @brief CircleShape object that represents a map node in an SFML window
   * 
   */
  sf::CircleShape nodeCircle;
  /**
   * @brief char representing the type of node ('b' for building, 'i' for intersection)
   * 
   */
  char type;
  /**
   * @brief name of the location the node represents
   * 
   */
  std::string name; 
};



#endif