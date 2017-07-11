/*
 * utitlity.h
 *
 *  Created on: Mar 27, 2015
 *      Author: Tadeze
 */
#ifndef UTILITY_HPP_
#define UTILITY_HPP_
#include<iostream>
#include<fstream>
#include<sstream>
#include <cmath>
#include <cstdlib>
#include<ctime>
#include<algorithm>
#include<queue>
#include<string>
#include<iterator>
#include<vector>
#include<algorithm>
#include<map>
#include<set>
#include<random>
#include<utility>
#include "cincl.hpp"
/*
 struct Data
 {
 int ncol;
 int nrow;
 std::vector<std::vector<double> > data;
 };
 */

//default_random_engine gen(time(NULL));
int randomI(int min, int max);
int randomEx(int min, int max, std::set<int>& exlude);
void sampleI(int min, int max, int nsample, std::vector<int> &sampleIndx);
double avgPL(int n);
double randomD(double min, double max);
template<typename T>
T randomT(T min, T max);
void swapInt(int a, int b, int* x);

//template<typename T>
double mean(std::vector<double> points);

std::vector<std::vector<double> > readcsv(const char* filename, char delim,
bool header);
//extern std::ofstream ffile; //("log.txt");
std::map<double, double> ecdf(std::vector<double> points);
std::vector<double> ADdistance(const std::vector<std::vector<double> > &depths,
bool weightToTail);
//extern doubleframe* dt;
//log file
extern std::ofstream logfile;
extern std::string tmpVar;
double score(double depth, int n);


int dblcmp(double *a, double *b);

//extern Data *dt;
#endif
/* UTITLITY_H_ */

