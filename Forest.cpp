/*
 * Forest.cpp
 *
 *  Created on: Aug 12, 2015
 *      Author: tadeze
 */

#include "Forest.hpp"

double Forest::getdepth(double* inst, Tree* tree) {
	return tree->pathLength(inst);
}

/*
 * Accepts single point (row) and return Anomaly Score
 */
double Forest::instanceScore(double *inst) {

	double avgPathLength = mean(pathLength(inst));
//	double scores = pow(2, -avgPathLength / avgPL(this->nsample));
	return avgPathLength;//scores;

}

/*
 * Score for  a set of dataframe in dataset
 */
std::vector<double> Forest::AnomalyScore(doubleframe* df) {
	std::vector<double> scores;
	//iterate through all points
	for (int inst = 0; inst < df->nrow; inst++) {
		scores.push_back(instanceScore(df->data[inst]));
	}
	return scores;
}
/*
 * Return instance depth in all trees
 */

std::vector<double> Forest::pathLength(double *inst) {
	std::vector<double> depth;
	for (std::vector<Tree*>::iterator it = this->trees.begin();
			it != trees.end(); ++it) {

		depth.push_back((*it)->pathLength(inst));

	}
	// pnt++;   //for logging purpose
	return depth;
}

/* PathLength for all points
 */
std::vector<std::vector<double> > Forest::pathLength(doubleframe* data) {
	std::vector<std::vector<double> > depths;
	for (int r = 0; r < data->nrow; r++)
		depths.push_back(pathLength(data->data[r]));
	return depths;
}
/*
 * Anderson_Darling test from the pathlength
 */
/*
 vector<double> IsolationForest::ADtest(const vector<vector<double> > &pathlength,bool weighttotail)
 {

 //Will fix later
 return ADdistance(pathlength,weighttotail);
 }*/
/* Compute the feature importance of a point
 * input: *inst data instance
 * output: feature importance
 * status: Incomplete!!
 */
std::vector<double> Forest::importance(double *inst) {
	//Need to be re-implemented
	//Incorrect code
	std::vector<double> depth;
	for (std::vector<Tree*>::iterator it = this->trees.begin();
			it != trees.end(); ++it) {

		depth.push_back(ceil((*it)->pathLength(inst)));

	}
	return depth;
}

void Forest::getSample(std::vector<int> &sampleIndex, int nsample,
		bool rsample, int nrow) {
	if(nsample > nrow)
		nsample = nrow;
	int *rndIdx = new int[nrow];
	for(int i = 0; i < nrow; ++i)
		rndIdx[i] = i;
	for(int i = 0; i < nsample; ++i){
		int r = rand() % nrow;
		int t = rndIdx[i];
		rndIdx[i] = rndIdx[r];
		rndIdx[r] = t;
	}
	for(int i = 0; i < nsample; ++i){
		sampleIndex.push_back(rndIdx[i]);
	}
	delete []rndIdx;
}

void Forest::printStat(std::ofstream &out){
	for(int i = 0; i < this->ntree; ++i){
		this->trees[i]->printDepthAndNodeSize(out);
		out << "#####################################" << std::endl;
	}
}
