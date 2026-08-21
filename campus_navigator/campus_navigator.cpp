/*
 * campus_navigator.cpp
 *
 * Campus navigator tool for Colorado School of Mines
 *
 * Author: Francisco Pineda
 */



#include "campus_navigator_functions.h"
#include "map_node.h"
#include "weighted_graph.h"

#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

using namespace sf;
using namespace std;



int main() {
  

  //hard coded integer for the number of locations in custom dataset
  const int numLocations = 153;

  //floats to hold the radius given to building and intersection circles when drawing
  const float buildingRadius = 12;
  const float intersectionRadius = 7;


  //create a weighted graph with as many vertices as there are locations
  WeightedGraph campusGraph(numLocations);
  


  //vector of map node structs that stores information about each node
  vector<MapNode> mapNodes(numLocations);

  //map from the name of a node to an integer id
  unordered_map<string, int> nameToID;


  //read vertex data
  bool successVertexData = read_vertex_data(mapNodes, nameToID, buildingRadius, intersectionRadius);

  //check if file reading was successful using the returned value
  if (!successVertexData) {
    return -1;
  }



  //read edge data
  bool successEdgeData = read_edge_data(campusGraph, nameToID, mapNodes);

  //check if file reading was successful using the returned value
  if (!successEdgeData) {
    return -1;
  }



  //create a reverse "map" of integer id's back to their name string
  //since id's are unique integers starting at 0, the index of a vector can implicitly map to the corresponding location name
  vector<string> idToName(numLocations);

  for (auto it = nameToID.begin(); it != nameToID.end(); it++) {
    //set the index of the current id to equal the corresponding string
    idToName.at(it->second) = it->first;
  }


  //original size of campus map image being used
  const float imageWidth = 922;
  const float imageHeight = 875 + 100; //add 100 pixels of vertical height for the text box

  //define a scaling factor to shrink the image down
  const float scale = 0.90f;


  //RenderWindow object with dimensions of scaled down campus image to ensure the window fits on standard displays
  RenderWindow window(VideoMode(imageWidth * scale, imageHeight * scale), "Campus Navigator");

  //map smaller window to original campus map image dimensions so coordinates remain unchanged
  View view(FloatRect(0, 0, imageWidth, imageHeight));
  window.setView(view);
 

  //set up the background image of the window
  Texture backgroundImage;

  if (!backgroundImage.loadFromFile("campus_map/campus_map.png")) {
    cerr << "Error: could not open background image for window" << endl;
    return -1;
  }

  Sprite background(backgroundImage);


  //setup for animation process

  //vector that holds the shortest path of the most recent search
  vector<int> shortestPath;

  //stack to hold vertices received from finding the shortest path between vertices
  //a search animation is done once the stack has one element left in it
  stack<int> shortestPathStack;

  //vector of RectangleShape objects to store which edges get drawn in SFML loop
  vector<RectangleShape> edgesVec;

  //integers to store the id's of the start and end vertices across SFML iterations
  int startID = -1;
  int endID = -1;


  //sf::Clock object to animate only after certain time has passed
  Clock animationClock;

  //use a 0.5 second delay when animating
  const float delay = 0.5;


  //declare font
  Font font;

  //load font from file
  if(!font.loadFromFile("font/Times.ttf")) {
    cerr << "Error: could not open font file" << endl;
    return -1;
  }

  //create text object
  Text botWindowText;

  //set the font
  botWindowText.setFont(font);
  
  //set the character size
  botWindowText.setCharacterSize(20);

  //set the style
  botWindowText.setStyle(Text::Regular);

  //set the color
  botWindowText.setFillColor(Color::White);

  //set the position to be in the bottom of the screen
  botWindowText.setPosition(Vector2f(70.f, 890.f));

  //set the text to a welcome message
  botWindowText.setString("Welcome to a campus navigator for Colorado School of Mines. You can click on buildings to select\nstart and end locations, press \"space\" to reset the map after a search, and exit with \"esc\" or \"q\".\nPress \"enter\" to continue");

  //bool to know if should output intro message
  bool inIntro = true;



  //SFML loop
  while (window.isOpen()) {
    //process events
    Event event;

    while (window.pollEvent(event)) {
      //check for closing window
      if ( event.type == Event::Closed 
      || Keyboard::isKeyPressed(Keyboard::Escape) 
      || Keyboard::isKeyPressed(Keyboard::Q) ) {
        window.close();



      } else if (event.type == Event::MouseButtonPressed 
      && (startID == -1 || endID == -1)
      && !inIntro) { //check for a user click within a map node, no search is active, and not in intro

        //get the position of the click
        Vector2i clickPos(event.mouseButton.x, event.mouseButton.y);

        //translate the coordinates of the click to the original campus map space
        Vector2f adjustedPos = window.mapPixelToCoords(clickPos);

        //get coordinates of mouse button press
        float mousePressX = adjustedPos.x;
        float mousePressY = adjustedPos.y;


        //have the click threshold of being within a circle be the radius of buildings, 12
        const double threshold = buildingRadius;

        //loop through each map node, check if the click is within the circle object of the node
        for (size_t i = 0; i < mapNodes.size(); i++) {
          //get the position of the current map node
          Vector2f currNodePos = mapNodes.at(i).nodeCircle.getPosition();

          //calculate distance between mouse press and current map node
          double distance = hypot(mousePressX - currNodePos.x, mousePressY - currNodePos.y);
          
          //if within the threshold, this node was clicked
          if (distance < threshold) {

            //check if node is a building and not an intersection
            if (mapNodes.at(i).type == 'b') {

              //first check if a start vertex has been assigned, and if not, this is the starting vertex
              if (startID == -1) {
                //assign the current vertex, i, to be the start
                startID = i;

                //set the color of the circle object to be green
                mapNodes.at(i).nodeCircle.setFillColor(Color(Color::Green.r, Color::Green.g, Color::Green.b, 150));

                //set the output text to inform the user to select and end location
                botWindowText.setString(idToName.at(startID) + " selected as start location. Click on the end location.\nPress \"esc\" or \"q\" at any time to exit");

                //since no other end node will be found in the map nodes vector as being clicked, break
                break;
              }

              //second, check if an end vertex has been chosen, and if the current vertex in question is not the same as the start vertex
              if (endID == -1 && static_cast<int>(i) != startID) {
                //assign the current vertex, i, to be the end
                endID = i;

                //set the color of the circle object to be red
                mapNodes.at(i).nodeCircle.setFillColor(Color(Color::Red.r, Color::Red.g, Color::Red.b, 150));

                //since no other end node will be found in the map nodes vector as being clicked, break
                break;
              }

            }
          }
        }

        //if both a start and end vertex were chosen, find the shortest path between them
        if (startID != -1 && endID != -1) {

          //set the output text to inform the user that a search animation is being done
          botWindowText.setString("Displaying path from " + idToName.at(startID) + " to " + idToName.at(endID) + "...");

          //find the shortest path between the two nodes chosen
          shortestPath = campusGraph.find_shortest_path(startID, endID);

          
          //if an empty vector was returned, no path was found, search animation is over
          if (shortestPath.size() == 0) {
            //set the output text to inform the user no path was found, and that they can reset the map
            botWindowText.setString("Path from " + idToName.at(startID) + " to " + idToName.at(endID) + " not found.\nYou can reset the map and start a new search by pressing \"space\"");
            
          } else { //otherwise, insert the vertices into a stack, in reverse order, for animating the search in future SFML loop iterations
            for (size_t i = shortestPath.size() - 1; i > 0; i--) {
              shortestPathStack.push(shortestPath.at(i));
            }
            shortestPathStack.push(shortestPath.at(0));
          }

        }



      //check if, once a search is done, the user presses space to reset the map, and start and end ID's have not been reset yet
      } else if (event.type == Event::KeyPressed 
      && event.key.code == Keyboard::Space
      && shortestPathStack.size() <= 1
      && startID != -1
      && endID != -1) {

        //reset the vector of edge rectangles
        edgesVec = {};

        //reset the stack of shortest vertices
        shortestPathStack = stack<int>();


        //if the shortest path vector is empty, no path was found, only the start and end nodes need the color reset
        if (shortestPath.size() == 0) {
          mapNodes.at(startID).nodeCircle.setFillColor(Color(Color::Yellow.r, Color::Yellow.g, Color::Yellow.b, 150));
          mapNodes.at(endID).nodeCircle.setFillColor(Color(Color::Yellow.r, Color::Yellow.g, Color::Yellow.b, 150));
        
        } else { //otherwise, use the shortest path vector to reset the colors of the vertices
          
          for (size_t i = 0; i < shortestPath.size(); i++) {
            //set the color based on whether building or intersection
            if (mapNodes.at(shortestPath.at(i)).type == 'b') {
              mapNodes.at(shortestPath.at(i)).nodeCircle.setFillColor(Color(Color::Yellow.r, Color::Yellow.g, Color::Yellow.b, 150));
            } else {
              mapNodes.at(shortestPath.at(i)).nodeCircle.setFillColor(Color::Black);
            }
          }

        }

        
        //reset the shortest path vector to prepare for the next search
        shortestPath = {};

        //reset the start and end ID's
        startID = -1;
        endID = -1;

        //set the output text to inform the user they can start another search
        botWindowText.setString("Map has been reset, click on the new starting location\nPress \"esc\" or \"q\" at any time to exit.");



      } else if (event.type == Event::KeyPressed
      && event.key.code == Keyboard::Enter
      && inIntro) { //check if user presses "enter" when in the intro message
        //change the inIntro boolean to false
        inIntro = false;
        
        //change the text position
        botWindowText.setPosition(Vector2f(15.f, 890.f));

        //change the text size
        botWindowText.setCharacterSize(18);

        //change the string to be ready for rest of program
        botWindowText.setString("Click on the start location\nPress \"esc\" or \"q\" at any time to exit");
      
      

      } else if (event.type == Event::Resized) { //check if user tried resizing window

        //prevent users from being able to resize the window to keep window fully visible and in original aspect ratio
        window.setSize(Vector2u(imageWidth * scale, imageHeight * scale));

      }
    }


    //clear screen
    window.clear();


    //draw the background
    window.draw(background);


    //draw the text
    window.draw(botWindowText);



    //check if any new edges need to be drawn (if the stack is size > 1)
    //only animate if enough time has passed since the last draw
    if (shortestPathStack.size() > 1 && animationClock.getElapsedTime().asSeconds() > delay) {

      //get the map node associated with the top element of the stack
      int firstNodeID = shortestPathStack.top();

      //pop the top node
      shortestPathStack.pop();

      //get the map node associated with the new top element
      int secondNodeID = shortestPathStack.top();
      //this second map node will be the first vertex in the next edge, not popped yet


      //draw an edge between the first map node and the second map node, and change the color of the second node

      //declare rectangle object
      RectangleShape currEdgeRec;

      //calculate the distance between the edges to set the width of the rectangle
      float width = get_node_distance(mapNodes.at(firstNodeID).nodeCircle, mapNodes.at(secondNodeID).nodeCircle);

      //use a fixed height for the triangle
      float height = 5.f;
      
      //set the dimensions of the rectangle
      currEdgeRec.setSize(Vector2f(width, height));

      //set the origin to be the center of the rectangle
      currEdgeRec.setOrigin(Vector2f(width/2, height/2));

      //set the position to be in the center between the two vertices
      currEdgeRec.setPosition((mapNodes.at(firstNodeID).nodeCircle.getPosition().x + mapNodes.at(secondNodeID).nodeCircle.getPosition().x) / 2, (mapNodes.at(firstNodeID).nodeCircle.getPosition().y + mapNodes.at(secondNodeID).nodeCircle.getPosition().y) / 2);

      //set the color to be blue
      currEdgeRec.setFillColor(Color::Blue);

      //find change in x and y between the nodes
      double changeX = mapNodes.at(firstNodeID).nodeCircle.getPosition().x - mapNodes.at(secondNodeID).nodeCircle.getPosition().x;
      double changeY = mapNodes.at(firstNodeID).nodeCircle.getPosition().y - mapNodes.at(secondNodeID).nodeCircle.getPosition().y;

      //calculate the angle between the nodes
      double angleRad = atan2(changeY, changeX);

      //convert angle from radians to degrees
      const double PI = 3.141592653;
      double angleDeg = angleRad * (180 / PI);

      //set the rotation of the rectangle
      currEdgeRec.setRotation(angleDeg);


      //add the edge rectangle for these vertices to a vector to be drawn later
      edgesVec.push_back(currEdgeRec);



      //set the second node ("to" in the edge) color based on if building or intersection
      //if the node is the end node, its color remains unchanged
      if (secondNodeID != endID) {
        if (mapNodes.at(secondNodeID).type == 'b') {
          mapNodes.at(secondNodeID).nodeCircle.setFillColor(Color(Color::Magenta.r, Color::Magenta.g, Color::Magenta.b, 150));
        } else {
          mapNodes.at(secondNodeID).nodeCircle.setFillColor(Color(Color::Magenta));
        }
      }


      //if the stack size became 1 this iteration, the animation is done
      if (shortestPathStack.size() == 1) {
        //set the output text to inform the user that the search animation is done
        botWindowText.setString("Path from " + idToName.at(startID) + " to " + idToName.at(endID) + " is complete.\nYou can reset the map and start a new search by pressing \"space\"");
      }



      //restart the clock
      animationClock.restart();

    }


    //draw the edge rectangles
    for (size_t i = 0; i < edgesVec.size(); i++) {
      window.draw(edgesVec.at(i));
    }


    //draw the node circles
    for (size_t i = 0; i < mapNodes.size(); i++) {
      window.draw(mapNodes.at(i).nodeCircle);
    }


    //update the window
    window.display();



  }



  return 0;
}