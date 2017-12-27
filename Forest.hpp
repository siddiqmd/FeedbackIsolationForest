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
	void writeScoreDatabase(doubleframe *dtTestNorm, doubleframe *dtTestAnom, char fName[]);
	void writeScores(doubleframe *dt, char fNamesuf[]);

	std::vector<int> getSeqMarExplanation(const double *inst, int dim, bool **exclude, int k = 0);
	std::vector<int> getSeqDropExplanation(const double *inst, int dim, bool **exclude, int k = 0);
	std::vector<int> getRevSeqMarExplanation(const double *inst, int dim, bool **exclude);
	std::vector<int> getRevSeqDropExplanation(const double *inst, int dim, bool **exclude);

	virtual double instanceMarginalScore(const double *inst, bool **marginalize);
	virtual double instanceScore(double *inst);
	std::vector<double> AnomalyScore(const doubleframe* df, bool **marginalize);
	std::vector<double> AnomalyScore(doubleframe* df);
	double getL2Norm2(double *inst);
	virtual std::vector<double> pathLength(double *inst);
	std::vector<std::vector<double> > pathLength(doubleframe* data);
	std::vector<double> ADtest(
			const std::vector<std::vector<double> > &pathlength,
			bool weighttotail);
	std::vector<double> importance(double *inst);
	double getdepth(double *inst, Tree* tree);
	void getSample(std::vector<int> &sampleIndex, int nsample,
			bool rSample, int nrow, int *rndIdx);
	void printStat(std::ofstream &out);
	void printPatternFreq(const doubleframe *df, int n, std::ofstream &out);
	void updateWeights(std::vector<double> &scores, double *inst, int direction, double lrate, double nsamp);
	void updateWeights(std::vector<double> &scores, double *inst, int direction, int type);
	void updateWeightsRunAvg(std::vector<double> &scores, double *inst, int direction);
	void updateWeightsPassAggr(std::vector<double> &scores, double *inst, int direction, double change, bool reg);
	void indexInstancesIntoNodes(const doubleframe* df);
	std::vector<double> anomalyScoreFromWeights(doubleframe* df);
	void weightIndexedScore(std::vector<double> &scores);
	double instanceScoreFromWeights(double *inst);
};
#endif /* FOREST_H_ */
