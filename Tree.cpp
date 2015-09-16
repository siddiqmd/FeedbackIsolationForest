/*
 * Tree.cpp
 *
 *  Created on: Mar 24, 2015
 *      Author: Tadeze
 */

#include "Tree.hpp"

void Tree::iTree(std::vector<int> const &dIndex, const doubleframe *dt,
		int height, int maxheight, bool stopheight) {
	this->depth = height; // Tree height
	this->nodeSize = dIndex.size(); // Set size of the node
	if (dIndex.size() <= 1 || (stopheight && this->depth > maxheight)) {
		return;
	}

	//*** Need modification
	//Initialize minmax array for holding max and min of an attributes
	std::vector<std::vector<double> > minmax;
	std::vector<double> tmp;

	for (int j = 0; j < dt->ncol; j++) {
		tmp.push_back(dt->data[dIndex[0]][j]);
		tmp.push_back(dt->data[dIndex[0]][j]);
		minmax.push_back(tmp); //initialize max and min to random value
		tmp.clear();
	}

	//Compute max and min of each attribute
	for (unsigned i = 0; i < dIndex.size(); i++) {
		//vector<double> inst = data->data[i];
		for (int j = 0; j < dt->ncol; j++) {
			if (dt->data[dIndex.at(i)][j] < minmax[j].at(0))
				minmax[j].at(0) = dt->data[dIndex.at(i)][j];
			if (dt->data[dIndex.at(i)][j] > minmax.at(j).at(1))
				minmax[j].at(1) = dt->data[dIndex.at(i)][j];
		}

	}

	//use only valid attributes
	std::vector<int> attributes;
	for (int j = 0; j < dt->ncol; j++) {
		if (minmax[j][0] < minmax[j][1]) {
			attributes.push_back(j);
		}
	}
	//return if no valid attribute found
	if (attributes.size() == 0)
		return;
	//Randomly pick an attribute and a split point

	//int randx = randomI(0, attributes.size());

	this->splittingAtt = attributes[randomI(0, attributes.size() - 1)]; //randx];
	this->splittingPoint = randomD(minmax[this->splittingAtt][0],
			minmax[this->splittingAtt][1]);

	//Split the node into two
	std::vector<int> lnodeData, rnodeData;
	double maxVal = minmax[this->splittingAtt][1];
	for (unsigned int i = 0; i < dIndex.size(); i++) {
		double curVal = dt->data[dIndex.at(i)][splittingAtt];
		if (curVal <= splittingPoint && curVal != maxVal) {
			lnodeData.push_back(dIndex.at(i));
		} else {
			rnodeData.push_back(dIndex.at(i));
		}
	}

	leftChild = new Tree();
	leftChild->parent = this;
	leftChild->iTree(lnodeData, dt, this->depth + 1, maxheight, stopheight);

	rightChild = new Tree();
	rightChild->parent = this;
	rightChild->iTree(rnodeData, dt, this->depth + 1, maxheight, stopheight);
}

/*
 * takes an instance as vector of double
 */
double Tree::pathLength(double *inst) {
	if ((this->leftChild == NULL && this->rightChild == NULL) || this->nodeSize <= 0) {
		return avgPL(this->nodeSize);
	}
	if (inst[this->splittingAtt] <= this->splittingPoint) {
		return this->leftChild->pathLength(inst) + 1.0;
	} else {
		return this->rightChild->pathLength(inst) + 1.0;
	}
}

// for online IF
void Tree::renewNodeSize(){
	nodeSize = newNodeSize;
	newNodeSize = 0;
	if(leftChild == NULL && rightChild == NULL)
		return;
	leftChild->renewNodeSize();
	rightChild->renewNodeSize();
}

void Tree::update(const double inst[]){
	++newNodeSize;
	if(leftChild == NULL && rightChild == NULL)
		return;
	if(inst[splittingAtt] <= splittingPoint)
		leftChild->update(inst);
	else
		rightChild->update(inst);
}

void Tree::printDepthAndNodeSize(std::ofstream &out){
	if(this->leftChild == NULL && this->rightChild == NULL){
		out << this->depth << "," << this->nodeSize << std::endl;
	}else{
		this->leftChild->printDepthAndNodeSize(out);
		this->rightChild->printDepthAndNodeSize(out);
	}
}
