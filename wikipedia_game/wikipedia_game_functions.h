/*
 * wikipedia_game_functions.h
 *
 * Declaration of functions used by Wikipedia game solver.
 * 
 * Author: Francisco Pineda
 */

#ifndef WIKIPEDIA_GAME_FUNCTIONS_H
#define WIKIPEDIA_GAME_FUNCTIONS_H

#include "unweighted_graph.h"

#include <string>
#include <unordered_map>



/**
 * @brief set the file path strings for Wikipedia data if the files exist
 * 
 * @param pagesFilePath string to hold page data file path
 * @param linksFilePath string to hold links data file path
 * @return true if a suitable dataset was successfully found
 * @return false if no suitable dataset was found
 */
bool set_file_path_strings(std::string& pagesFilePath, std::string& linksFilePath);
/**
 * @brief extract title field from file
 * 
 * @param dataFileLine string of line from file
 * @param titleString string to hold the title
 * @param strCounter counter through file line string
 */
void extract_title_field(const std::string& dataFileLine, std::string& titleString, size_t& strCounter);
/**
 * @brief extract and process a line from the page data csv
 * 
 * @param titleToID map from titles of pages to an integer id
 * @param pagesFileLine line from page data csv being processed
 * @param counter integer id of graph vertex being assigned
 */
void extract_page_data_line(std::unordered_map<std::string, int>& titleToID, const std::string& pagesFileLine, int& counter);
/**
 * @brief read the page data csv of Wikipedia pages
 * 
 * @param titleToID map from titles of pages to an integer id
 * @param pagesFilePath string with file path to page data
 */
void read_page_data(std::unordered_map<std::string, int>& titleToID, const std::string& pagesFilePath);
/**
 * @brief adds an edge to the unweighted graph, representing a link between two pages
 * 
 * @param wikiGraph graph object to add edge to
 * @param currSourceTitle name of source page
 * @param currTargetTitle name of target page
 * @param titleToID map of page titles to an integer id
 */
void add_wiki_edge(UnweightedGraph& wikiGraph, const std::string& currSourceTitle, const std::string& currTargetTitle, const std::unordered_map<std::string, int>& titleToID);
/**
 * @brief extract and process a line from the links data csv
 * 
 * @param wikiGraph graph object to add edge to
 * @param titleToID map from titles of pages to an integer id
 * @param linksFileLine line from links data csv being processed
 */
void extract_links_data_line(UnweightedGraph& wikiGraph, const std::unordered_map<std::string, int>& titleToID, const std::string& linksFileLine);
/**
 * @brief read the links data csv for Wikipedia pages
 * 
 * @param wikiGraph graph representing Wikipedia pages and links
 * @param titleToID map from titles of pages to an integer id
 * @param linksFilePath string with file path to links data
 */
void read_links_data(UnweightedGraph& wikiGraph, const std::unordered_map<std::string, int>& titleToID, const std::string& linksFilePath);
/**
 * @brief get a page from the user
 * 
 * @param pageStr string to hold page
 * @param quitCurrInstance boolean to indicate if user wants to quit
 * @param titleToID map from titles of pages to an integer id
 */
void get_page_from_user(std::string& pageStr, bool& quitCurrInstance, const std::unordered_map<std::string, int>& titleToID);



#endif