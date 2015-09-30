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
public:
	static long total; // counts number of nodes access
	static long numDrifts; // counts number of instances crossing the boundary of min and max
	static long numDriftsHandled; // counts number of instances crossing handled outside of boundary
	static bool checkRange;

	Tree() {
		leftChild = NULL;
		rightChild = NULL;
		parent = NULL;
		splittingAtt = -1;
		splittingPoint = 0;
		depth = 0;
		nodeSize = 0;
		newNodeSize = 0;
		minAttVal = 0;
		maxAttVal = 0;
	}

	virtual ~Tree() {
		delete leftChild;
		delete rightChild;
	}

	void iTree(std::vector<int> const &dIndex, const doubleframe* dt,
			int height, int maxHeight, bool stopheight);
	double pathLength(double *inst);
	// for online IF
	void renewNodeSize();
	void update(const double inst[]);
	void printDepthAndNodeSize(std::ofstream &out);
};

#endif /* TREE_H_ */
