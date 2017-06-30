/*
 * Tree.h
 *
 *  Created on: Mar 24, 2015
 *      Author: Tadeze
 */

#ifndef TREE_H_
#define TREE_H_
#include "utility.hpp"
#include "cincl.hpp"

class Tree {
	Tree *leftChild;
	Tree *rightChild;
	Tree *parent;
	int depth;
	int nodeSize;

	int splittingAtt;
	double splittingPoint;

	int newNodeSize; // new attribute for online IF
	double minAttVal, maxAttVal;
	double volume;// store volume of this node in log scale
	static std::vector<double> LB, UB;

public:
	static bool checkRange;
	static bool useVolumeForScore;

	Tree() {
		leftChild = NULL;
		rightChild = NULL;
		parent = NULL;
		splittingAtt = -1;
		splittingPoint = 0;
		depth = 0;
		nodeSize = 1;

		newNodeSize = 1;// prior 1 in case of no data
		minAttVal = 0;
		maxAttVal = 0;
		volume = 0;
	}

	virtual ~Tree() {
		delete leftChild;
		delete rightChild;
	}

	void iTree(std::vector<int> const &dIndex, const doubleframe* dt,
			int &maxHeight);
	double pathLength(double *inst);
	double pathLength(const double *inst, const bool *marginalize);
	// for online IF
	void renewNodeSize();
	void update(const double inst[]);
	void printDepthAndNodeSize(std::ofstream &out);
	void printPatternFreq(double inst[], int &tid, int &iid, std::ofstream &out);

	std::vector<double> getPatternScores(double *inst, int depLim = 10);
	double getScoreAtDepth(double *inst, int depLim);
	double getPatternScoreAtDepth(double *inst, int depLim);

	static void initialezeLBandUB(const doubleframe* _df, std::vector<int> &sampleIndex);
};

#endif /* TREE_H_ */
