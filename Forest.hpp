/*
 * Forest.h
 *
 *  Created on: Aug 12, 2015
 *      Author: tadeze
 */

#ifndef FOREST_H_
#define FOREST_H_
#include "utility.hpp"
#include "Tree.hpp"
#include "cincl.hpp"

class Forest {
public:
	std::vector<Tree*> trees;
	int ntree;bool rsample;
	int nsample;
	int maxheight;

	Forest() {
		rsample = false;
		ntree = 0;
		nsample = 256;
	}
	Forest(int _ntree, int _nsample, int _maxheight,
			bool _rsample) {
		ntree = _ntree;
		nsample = _nsample;
		maxheight = _maxheight;
		rsample = _rsample;
	}
	virtual ~Forest() {
		for (std::vector<Tree*>::iterator it = trees.begin(); it != trees.end();
				++it) {
			delete (*it);
		}
	}

	std::vector<double> getScore(doubleframe *df, int type);

	virtual double instanceScore(double *inst);
	std::vector<double> AnomalyScore(doubleframe* df);
	virtual std::vector<double> pathLength(double *inst);
	std::vector<std::vector<double> > pathLength(doubleframe* data);
	std::vector<double> ADtest(
			const std::vector<std::vector<double> > &pathlength,
			bool weighttotail);
	std::vector<double> importance(double *inst);
	double getdepth(double *inst, Tree* tree);
	void getSample(std::vector<int> &sampleIndex, int nsample,
			bool rSample, int nrow);
	void printStat(std::ofstream &out);
	void printPatternFreq(const doubleframe *df, int n, std::ofstream &out);
};
#endif /* FOREST_H_ */
