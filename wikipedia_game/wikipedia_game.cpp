/*
 * wikipedia_game.cpp
 *
 * Wikipedia game solver using dataset with 100k pages and ~28 million links.
 *
 * Author: Francisco Pineda
 */



#include "unweighted_graph.h" 
#include "wikipedia_game_functions.h" 

#include <chrono>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;



int main() {

  //set up random number generation
  random_device rd;
  mt19937 mt(rd());


  //print intro messages
  cout << "Welcome to a Wikipedia game solver\nThis program will find the shortest path from one Wikipedia page to another via links between pages" << endl;
  cout << endl;
  cout << "Attempting to generate graph from Wikipedia data of 100,000 pages and 28,855,738 links..." << endl;
  cout << endl;

  //start a clock to keep track of setup time
  auto setupStartTime = chrono::high_resolution_clock::now();


  //for mapping the article titles to their integer id
  unordered_map<string, int> titleToID;

  //string variables to hold which files to open
  string pagesFilePath;
  string linksFilePath;

  //check whether full dataset can be loaded, or if sample dataset should be loaded, and set the file paths as needed
  bool successfullyFounData = set_file_path_strings(pagesFilePath, linksFilePath);

  //check if data was not found
  if (!successfullyFounData) {
    return -1;
  }

  //read page data
  read_page_data(titleToID, pagesFilePath);


  //graph object to represent wikipedia pages and links
  //the number of vertices is how many pages the file contains, which can be found from the size of the map from titles to id's
  UnweightedGraph wikiGraph(titleToID.size());


  //read links data
  read_links_data(wikiGraph, titleToID, linksFilePath);



  //stop the clock keeping track of setup time
  auto setupStopTime = chrono::high_resolution_clock::now();

  //determine how long the setup took to output to the user
  auto setupDuration = chrono::duration_cast<chrono::milliseconds>(setupStopTime - setupStartTime);

  cout << "Setup complete (" << (setupDuration.count() / 1000.0) << " s)" << endl;
  cout << endl;

  
  //reverse "map" of id's to title strings
  //since id's are unique integers starting at 0, the index of a vector can implicitly map to the corresponding title
  vector<string> idToTitle(titleToID.size());

  for (auto it = titleToID.begin(); it != titleToID.end(); it++) {
    //set the index of the current id to equal the corresponding string
    idToTitle.at(it->second) = it->first;
  }




  //main loop
  while (true) {

    cout << "-----------------------------------------------------------------------------" << endl << endl;

    cout << "Reminder: there may not be a valid path between two pages" << endl;
    
    cout << endl;

    cout << "Options available: " << endl;
    cout << "Enter \"1\" to manually type in your choice of source and target page" << endl;
    cout << "Enter \"2\" to generate a list of random articles for you to choose from (you will choose how many articles you want to see)" << endl;
    cout << "Enter \"3\" to completely randomize the source and target pages" << endl;
    cout << "Enter \"quit\" to exit the program" << endl; 

    cout << endl;

    //string to hold user choice when read
    string userChoice;
    cout << "Enter the option you would like: ";

    getline(cin, userChoice);

    cout << endl;
    

    //store the source and target page id's to find a path between
    int userSourceId;
    int userTargetId;


    //case 1 or case 2: the user wants to manually enter source and target pages to find a path between
    //in case 2 specifically, the user wants a randomly generated list of pages to choose from
    if (userChoice == "1" || userChoice == "2") {
      //strings to store source and target page names entered by user
      string sourcePage;
      string targetPage;

      //boolean for keeping track of if user wants to quit this instance of entering pages
      bool quitCurrInstance = false;

      //user wants list of random pages to choose from
      if (userChoice == "2") {
        string numChoices;
        cout << "Enter how many options you want to choose from (or 0 if you want to exit this menu): ";

        int numChoicesInt = -1;

        //ensure input is larger than 0, less than the number of articles (100,000), and a valid integer
        do {
          getline(cin, numChoices);

          try {
            numChoicesInt = stoi(numChoices);

            if (numChoicesInt < 0 || numChoicesInt > 100000) {
              cerr << "Error: enter a number between 1 and 100,000: ";
              continue;
            }

            if (numChoicesInt == 0) {
              //set boolean for future update
              quitCurrInstance = true;
            }

          } catch (const invalid_argument& stoiError) { //handle exception if user entered something other than an integer
            cerr << "Error: input was not a valid number, try again: ";
            numChoicesInt = -1;
            continue;
          }

        } while (numChoicesInt < 0 || numChoicesInt > 100000);
        
        cout << endl;


        //user entered 0 choices above, ignore the rest of this loop iteration
        if (quitCurrInstance) {
          continue;
        }


        //generate random numbers from valid vertex id's of the graph
        uniform_int_distribution<int> dist(0, titleToID.size() - 1);
        
        //keep unordered set for existing id's to not repeat them
        unordered_set<int> randNumsSet;

        //print pages user can select from
        cout << "List of possible pages:" << endl; 
        for (int i = 0; i < numChoicesInt; i++) {
          //generate unique random page id's
          int randID;
          do {
            randID = dist(mt);
          } while (randNumsSet.count(randID) > 0);
          
          //add newly generated id to set
          randNumsSet.insert(randID);

          //print out the page title equivalent
          cout << idToTitle.at(randID) << endl;
        }
        cout << endl;
      }


      //regardless of whether random pages were printed, get a source title and target title from the user

      cout << "Enter the title of the source page, or \"quit\" to exit this menu: ";

      //get the source page from the user
      get_page_from_user(sourcePage, quitCurrInstance, titleToID);


      //if user entered "quit" above, ignore the rest of this iteration of the while loop
      if (quitCurrInstance) {
        cout << endl;
        continue;
      }

      cout << "\"" << sourcePage << "\" successfully set as source page" << endl;

      quitCurrInstance = false;

      cout << endl;
      cout << "Enter the title of the target page, or \"quit\" to exit this menu: ";

      //get the target page from the user
      get_page_from_user(targetPage, quitCurrInstance, titleToID);


      //if user entered quit above, ignore this iteration of the while true loop
      if (quitCurrInstance) {
        cout << endl;
        continue;
      }

      cout << "\"" << targetPage << "\" successfully set as target page" << endl;

      //get id's for the pages the user entered
      userSourceId = titleToID.find(sourcePage)->second;
      userTargetId = titleToID.find(targetPage)->second;
      
      
    //case 3: user wants random articles
    } else if (userChoice == "3") {

      //generate random numbers from valid integer id's in the graph
      uniform_int_distribution<int> dist(0, titleToID.size() - 1);

      //generate unique source and target id's
      userSourceId = dist(mt);
      do {
        userTargetId = dist(mt);
      } while (userTargetId == userSourceId);


    //case 4, user wants to quit
    } else if (userChoice == "quit") {
      cout << "Exiting..." << endl;
      break;

    //case 5, invalid input
    } else {
      cout << "Invalid input" << endl << endl;
      continue;
    }


    cout << endl << endl;
    cout << "Finding a path between \"" << idToTitle.at(userSourceId) << "\" and \"" << idToTitle.at(userTargetId) << "\"" << endl;
    cout << endl;
    cout << "..." << endl;
    cout << endl;


    //start a clock before the search
    auto searchStartTime = chrono::high_resolution_clock::now();

    //run bfs between the two pages to find the shortest path between them
    vector<int> pathVertices = wikiGraph.find_shortest_path(userSourceId, userTargetId);

    //stop the clock after the search
    auto searchStopTime = chrono::high_resolution_clock::now();

    //calculate duration of search
    auto searchDuration = chrono::duration_cast<chrono::milliseconds>(searchStopTime - searchStartTime);

    
    if (pathVertices.size() == 0) { //no path was found
      cout << "No path found" << endl;

    } else { //path found
      //print time it took for search
      cout << "Path found (" << (searchDuration.count() / 1000.0) << " s): " << endl;
      cout << endl;

      //convert the indices to their title strings with the id to title vector and output them
      for (size_t i = 0; i < pathVertices.size(); i++) {
        string currStr = idToTitle.at(pathVertices.at(i));
        if (i == 0) {
          cout << currStr << endl;
        } else {
          cout << "--> " << currStr << endl;
        }
        
      }

      cout << endl;

      //print the degrees of separation between the pages
      cout << "Degrees of separation: " << pathVertices.size() - 1 << endl;
    }
    
    cout << endl;


    //wait for user to interact with program before continuing
    string userContinue;

    cout << "Press \"enter\" to continue, or type \"quit\" to exit the program: ";
    getline(cin, userContinue); //getline blocks until user enters something
    if (userContinue == "quit") {
      break;
    }
    cout << endl;

  }



  return 0;
}

