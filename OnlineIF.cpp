#include "OnlineIF.hpp"

OnlineIF::OnlineIF(int _ntree, doubleframe* _df, int _nsample, int _maxheight,
		bool _rsample, int _windowSize) :
		Forest(_ntree, _nsample, _maxheight, _rsample) {

	this->windowSize = _windowSize;
	this->updateCount = 0;
	genInitTreeStructures(_df);
}

/*
 * Generate initial tree structures from offline isolation
 * forest using first windowSize instances
 */
void OnlineIF::genInitTreeStructures(doubleframe *dataset){
	std::vector<int> sampleIndex;
	for (int n = 0; n < this->ntree; n++) {
		// Sample and shuffle the data.
		sampleIndex.clear();
		getSample(sampleIndex, nsample, rsample, windowSize);

		Tree::initialezeLBandUB(dataset, sampleIndex);

		// build trees
		Tree *tree = new Tree();
		tree->iTree(sampleIndex, dataset, maxheight);
		this->trees.push_back(tree); //add tree to forest
	}
}

double OnlineIF::instanceScore(double *inst) {
	double avgPathLength = mean(pathLength(inst));
	double scores = pow(2, -avgPathLength / avgPL(this->nsample));
	return scores;
}


/*
 * Update corresponding nodes in the trees for a new instance
 */
void OnlineIF::update(double inst[]){
	double p = this->nsample/double(this->windowSize);
	for(int i = 0; i < this->ntree; ++i){
		if(randomD(0, 1) < p)
			this->trees[i]->update(inst);
	}

	this->updateCount++;

	// After performing windowSize updates renew trees with new nodeSizes
	if(this->updateCount == this->windowSize){
		for(int i = 0; i < this->ntree; ++i){
			this->trees[i]->renewNodeSize();
		}
		// reset update count to start again
		this->updateCount = 0;
	}
}
