/*
 * wikipedia_game_functions.cpp
 *
 * Definitions for the functions used by Wikipedia game solver.
 *
 * Author: Francisco Pineda
 */


#include "unweighted_graph.h"
#include "wikipedia_game_functions.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;



bool set_file_path_strings(string& pagesFilePath, string& linksFilePath) {
  //attempt to open full pages and link dataset
  ifstream pagesFile("wikipedia_data/pages_export.csv");
  ifstream linksFile("wikipedia_data/links_export.csv");

  //check if one or both full dataset files failed to open successfully
  if (pagesFile.fail() || linksFile.fail()) {

    //full dataset may not be downloaded, attempt to open sample dataset
    cerr << "Error: full dataset not found.\nAttempting to open sample dataset..." << endl << endl;

    //clear the ifstream objects
    pagesFile.clear();
    linksFile.clear();

    //attempt to open the sample dataset
    pagesFile.open("wikipedia_data/sample_pages_export.csv");
    linksFile.open("wikipedia_data/sample_links_export.csv");


    //check if one or both sample files failed to open successfully
    if (pagesFile.fail() || linksFile.fail()) {
      //sample dataset not found, no file could be opened
      cerr << "Error: sample dataset not found.\nNo suitable dataset found, exiting..." << endl;

      //return false to indicate no suitable dataset found
      return false;

    } else { //sample data successfully opened
      cout << "Sample dataset successfully opened" << endl << endl;

      //set the file strings to the sample data paths
      pagesFilePath = "wikipedia_data/sample_pages_export.csv";
      linksFilePath = "wikipedia_data/sample_links_export.csv";

      //return true for suitable dataset being found
      return true;
    }
    
  } else { //full data successfully opened
    //set the file strings to the full data paths
    pagesFilePath = "wikipedia_data/pages_export.csv";
    linksFilePath = "wikipedia_data/links_export.csv";

    //return true for suitable dataset being found
    return true;
  }
}



void extract_title_field(const string& dataFileLine, string& titleString, size_t& strCounter) {
  //case 1: title field is enclosed in a set of double quotes
  //the title string has a comma in it, or a string within the field is enclosed in quotes
  //loop until the closing double quote
  if (dataFileLine.at(strCounter) == '\"') {
    
    strCounter++; //move past the first quote

    while (strCounter < dataFileLine.size()) { 
      
      //if have two double quotes, add one quote to the title string
      if ((strCounter + 1 < dataFileLine.size()) && dataFileLine.at(strCounter) == '\"' && dataFileLine.at(strCounter + 1) == '\"') {
        titleString += "\"";
        strCounter += 2; //increment counter twice, one for each double quote

      } else if (dataFileLine.at(strCounter) == '\"') {
        //if have a one double quote, end of the title string
        return;

      } else { //regular character, add to string and increment counter once
        titleString += dataFileLine.at(strCounter);
        strCounter++;
      }

    }

  } else { //the string is not enclosed in quotes, ends at the next comma
    
    while (strCounter < dataFileLine.size()) { 
      if (dataFileLine.at(strCounter) == ',') {
        //if have a comma, end of title string
        return;

      } else { //regular character, add to string and increment counter once
        titleString += dataFileLine.at(strCounter);
        strCounter++;
      }
    }

  } 

}



void extract_page_data_line(unordered_map<string, int>& titleToID, const string& pagesFileLine, int& counter) {
  //counter through the line of the file
  size_t strCounter = 0;

  //string to form title
  string currTitle = "";

  //loop past the first field, id
  while (pagesFileLine.at(strCounter) != ',') {
    strCounter++;
  }

  //advance one more to move past first comma
  strCounter++;


  //strCounter is at the start of the title field

  //extract page title field
  extract_title_field(pagesFileLine, currTitle, strCounter);

  //insert the pair (title, id) into the map from titles to id's, and then increment the id counter
  titleToID.emplace(currTitle, counter);
  counter++;
}



void read_page_data(unordered_map<string, int>& titleToID, const string& pagesFilePath) {
  //the graph class uses integers starting at 0 to represent vertices
  //keep track of integer values starting at 0 to map title strings to vertices
  int counter = 0;

  //open page data file using string stored
  ifstream pagesFile(pagesFilePath);
  
  //string to hold line from file
  string pagesFileLine;

  //read in first line that only contains csv info
  getline(pagesFile, pagesFileLine);


  //loop through page data
  while (getline(pagesFile, pagesFileLine)) {
    //extract and process the line from the pages data
    extract_page_data_line(titleToID, pagesFileLine, counter);
  }

}



void add_wiki_edge(UnweightedGraph& wikiGraph, const string& currSourceTitle, const string& currTargetTitle, const unordered_map<string, int>& titleToID) {
  //search for source and target strings in map from titles to id's
  auto itSource = titleToID.find(currSourceTitle);
  auto itTarget = titleToID.find(currTargetTitle);

  //if one or both titles not found, do not insert an edge
  if (itSource == titleToID.end() || itTarget == titleToID.end()) {
    return;

  } else { //both source and target titles exist, form an edge between them in the graph
    int sourceID = itSource->second;
    int targetID = itTarget->second;

    wikiGraph.add_edge(sourceID, targetID);
  }
}



void extract_links_data_line(UnweightedGraph& wikiGraph, const unordered_map<string, int>& titleToID, const string& linksFileLine) {
  //counter through this line of the file
  size_t strCounter = 0;

  //string to form source title
  string currSourceTitle = "";


  //loop past the first field, id
  while (linksFileLine.at(strCounter) != ',') {
    strCounter++;
  }

  //advance one more to move past first comma
  strCounter++;


  //strCounter is at the start of the source title field

  //for tracking which case this field is, to adjust counter after extracting title
  bool quoteAtStart;

  if (linksFileLine.at(strCounter) == '\"') {
    quoteAtStart = true;
  } else {
    quoteAtStart = false;
  }


  //extract source title field
  extract_title_field(linksFileLine, currSourceTitle, strCounter);


  if (quoteAtStart) { 
    //counter ended on last quote, advance it past the quote and comma to be on start of next field
    strCounter += 2;

  } else {
    //counter ended on comma, advance it past the comma to be on start of next field
    strCounter++;
  }



  //strCounter is after the comma indicating the start of the target title field

  //string to form target title
  string currTargetTitle = "";

  //extract target title field
  extract_title_field(linksFileLine, currTargetTitle, strCounter);


  //add an edge in the graph between the vertices representing source and target pages
  add_wiki_edge(wikiGraph, currSourceTitle, currTargetTitle, titleToID);
}



void read_links_data(UnweightedGraph& wikiGraph, const unordered_map<string, int>& titleToID, const string& linksFilePath) {
  //open links data file using string stored
  ifstream linksFile(linksFilePath);

  //string to hold line from file
  string linksFileLine;

  //read in first line that only contains csv info
  getline(linksFile, linksFileLine);


  //loop through links data
  while (getline(linksFile, linksFileLine)) {
    //extract and process the line from the links data
    extract_links_data_line(wikiGraph, titleToID, linksFileLine);
  }

}



void get_page_from_user(string& pageStr, bool& quitCurrInstance, const unordered_map<string, int>& titleToID) {
  do { //loop until a valid page is entered, or user enters quit
    getline(cin, pageStr);

    //set boolean flag true for if user wants to quit
    if (pageStr == "quit") {
      quitCurrInstance = true;
      break;
    }

    //check if it is in title to id map, and if not, let user try to re-enter page
    if (titleToID.count(pageStr) == 0) {
      cerr << "Error: page does not exist in dataset (reminder: titles are case sensitive).\nTry another one, or enter \"quit\" to exit this menu: ";
    }

  } while (titleToID.count(pageStr) == 0);
}

