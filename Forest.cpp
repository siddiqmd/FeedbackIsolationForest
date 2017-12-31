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

double Forest::getL2Norm2(double *inst){
	double l2norm2 = 0;
	for(int i = 0; i < this->ntree; i++){
		l2norm2 += this->trees[i]->getHighestDepth(inst);
	}
	return l2norm2;
}

double Forest::getL1NormofWeights(){
	double L1norm = 0;
	for(int i = 0; i < this->ntree; i++){
		L1norm += this->trees[i]->getL1NormofWeights();
	}
	return L1norm;
}
/*
 * Accepts single point (row) and return Anomaly Score
 */
double Forest::instanceScore(double *inst) {

	double avgPathLength = mean(pathLength(inst));
	double scores = pow(2, -avgPathLength / avgPL(this->nsample));
	return scores;

// unnormalized raw depth score
//	double scores = pow(2, -avgPathLength / avgPL(this->nsample));
//	return avgPathLength;//scores;

}

/*
 * Compute sequential marginal explanation
 * inst: instance to be explained
 * dim: dimension of the data i.e. number of features in inst
 * k: number of most important features to find
 */
std::vector<int> Forest::getSeqMarExplanation(const double *inst, int dim, bool **exclude, int k){
	if(k == 0) k = dim;
	std::vector<int> explanation;
	double temp, max;
	int pick, pickQ;
	bool **marginalize = new bool *[dim];
	for(int i = 0; i < dim; ++i){
		marginalize[i] = new bool[4];
		for(int j = 0; j < 4; j++)
			marginalize[i][j] = true;
	}

	for(int cnt = 0; cnt < k; cnt++){
		max = -100;
		pick = -1;
		pickQ = -1;
		for(int i = 0; i < dim; ++i){
			int q = Tree::getQuantile(i, inst[i]);
			if(marginalize[i][q] == false || exclude[i][q] == true)
				continue;
			marginalize[i][q] = false;
			temp = instanceMarginalScore(inst, marginalize);
			marginalize[i][q] = true;
			if(temp > max){
				max = temp;
				pick = i;
				pickQ = q;
			}
		}
		if(pick >= 0){
			explanation.push_back(pick);
			marginalize[pick][pickQ] = false;
		}
	}

	for(int i = 0; i < dim; ++i)
		delete []marginalize[i];
	delete []marginalize;

	return explanation;
}

/*
 * Compute sequential dropout explanation
 * inst: instance to be explained
 * dim: dimension of the data i.e. number of features in inst
 * k: number of most important features to find
 */
std::vector<int> Forest::getSeqDropExplanation(const double *inst, int dim, bool **exclude, int k){
	if(k == 0) k = dim;
	std::vector<int> explanation;
	double temp, min;
	int pick, pickQ;
	bool **marginalize = new bool *[dim];
	for(int i = 0; i < dim; ++i){
		marginalize[i] = new bool[4];
		for(int j = 0; j < 4; j++)
			marginalize[i][j] = false;
	}

	for(int cnt = 0; cnt < k; cnt++){
		min = 100;
		pick = -1;
		pickQ = -1;
		for(int i = 0; i < dim; ++i){
			int q = Tree::getQuantile(i, inst[i]);
			if(marginalize[i][q] == true || exclude[i][q] == true)
				continue;
			marginalize[i][q] = true;
			temp = instanceMarginalScore(inst, marginalize);
			marginalize[i][q] = false;
			if(temp < min){
				min = temp;
				pick = i;
				pickQ = q;
			}
		}
		if(pick >= 0){
			explanation.push_back(pick);
			marginalize[pick][pickQ] = true;
		}
	}

	for(int i = 0; i < dim; ++i)
		delete []marginalize[i];
	delete []marginalize;

	return explanation;
}

/*
 * Compute reverse sequential marginal explanation
 * inst: instance to be explained
 * dim: dimension of the data i.e. number of features in inst
 * exclude: features/quantiles should excluded from explanation
 */
std::vector<int> Forest::getRevSeqMarExplanation(const double *inst, int dim, bool **exclude){
	std::vector<int> explanation;
	double temp, min;
	int pick, pickQ;
	bool **marginalize = new bool *[dim];
	for(int i = 0; i < dim; ++i){
		marginalize[i] = new bool[4];
		for(int j = 0; j < 4; j++)
			marginalize[i][j] = true;
	}

	for(int cnt = 0; cnt < dim; cnt++){
		min = 100;
		pick = -1;
		pickQ = -1;
		for(int i = 0; i < dim; ++i){
			int q = Tree::getQuantile(i, inst[i]);
			if(marginalize[i][q] == false || exclude[i][q] == true)
				continue;
			marginalize[i][q] = false;
			temp = instanceMarginalScore(inst, marginalize);
			marginalize[i][q] = true;
			if(temp < min){
				min = temp;
				pick = i;
				pickQ = q;
			}
		}
		if(pick >= 0){
			explanation.push_back(pick);
			marginalize[pick][pickQ] = false;
		}
	}

	// Reverse explanation
	int len = (int)explanation.size();
	for(int i = 0; i < len/2; i++){
		int temp = explanation[i];
		explanation[i] = explanation[len-i-1];
		explanation[len-i-1] = temp;
	}

	for(int i = 0; i < dim; ++i)
		delete []marginalize[i];
	delete []marginalize;

	return explanation;
}

/*
 * Compute reverse sequential dropout explanation
 * inst: instance to be explained
 * dim: dimension of the data i.e. number of features in inst
 * exclude: features/quantiles should excluded from explanation
 */
std::vector<int> Forest::getRevSeqDropExplanation(const double *inst, int dim, bool **exclude){
	std::vector<int> explanation;
	double temp, max;
	int pick, pickQ;
	bool **marginalize = new bool *[dim];
	for(int i = 0; i < dim; ++i){
		marginalize[i] = new bool[4];
		for(int j = 0; j < 4; j++)
			marginalize[i][j] = false;
	}

	for(int cnt = 0; cnt < dim; cnt++){
		max = -100;
		pick = -1;
		pickQ = -1;
		for(int i = 0; i < dim; ++i){
			int q = Tree::getQuantile(i, inst[i]);
			if(marginalize[i][q] == true || exclude[i][q] == true)
				continue;
			marginalize[i][q] = true;
			temp = instanceMarginalScore(inst, marginalize);
			marginalize[i][q] = false;
			if(temp > max){
				max = temp;
				pick = i;
				pickQ = q;
			}
		}
		if(pick >= 0){
			explanation.push_back(pick);
			marginalize[pick][pickQ] = true;
		}
	}

	// Reverse explanation
	int len = (int)explanation.size();
	for(int i = 0; i < len/2; i++){
		int temp = explanation[i];
		explanation[i] = explanation[len-i-1];
		explanation[len-i-1] = temp;
	}

	for(int i = 0; i < dim; ++i)
		delete []marginalize[i];
	delete []marginalize;

	return explanation;
}

double Forest::instanceMarginalScore(const double *inst, bool **marginalize){
	std::vector<double> depths;
	for (std::vector<Tree*>::iterator it = this->trees.begin();
			it != trees.end(); ++it) {
		depths.push_back((*it)->pathLength(inst, marginalize));
	}

	double avgPathLength = mean(depths);
	double scores = pow(2, -avgPathLength / avgPL(this->nsample));
	return scores;
}

double Forest::instanceScoreFromWeights(double *inst){
	std::vector<double> sums;
	for (std::vector<Tree*>::iterator it = this->trees.begin();
			it != trees.end(); ++it) {
		sums.push_back((*it)->getScoreFromWeights(inst));
	}
	double score = mean(sums);
	return score;
}

std::vector<double> Forest::anomalyScoreFromWeights(doubleframe* df) {
	std::vector<double> scores;
	//iterate through all points
	for (int inst = 0; inst < df->nrow; inst++) {
		scores.push_back(instanceScoreFromWeights(df->data[inst]));
	}
	return scores;
}

void Forest::weightIndexedScore(std::vector<double> &scores){
	for (std::vector<Tree*>::iterator it = this->trees.begin();
			it != trees.end(); ++it) {
		(*it)->weightIndexedScore(scores);
	}
}

void Forest::updateWeights(std::vector<double> &scores, double *inst, int direction, double lrate, double reg){
	for (std::vector<Tree*>::iterator it = this->trees.begin();
			it != trees.end(); ++it) {
		(*it)->updateWeights(scores, inst, direction, lrate, reg);
	}
}

void Forest::updateWeights(std::vector<double> &scores, double *inst, int direction, int type, double change, double reg){
	for (std::vector<Tree*>::iterator it = this->trees.begin();
			it != trees.end(); ++it) {
		(*it)->updateWeights(scores, inst, direction, type, change, reg);
	}
}

void Forest::updateWeightsRunAvg(std::vector<double> &scores, double *inst, int direction){
	for (std::vector<Tree*>::iterator it = this->trees.begin();
			it != trees.end(); ++it) {
		(*it)->updateWeightsRunAvg(scores, inst, direction);
	}
}

void Forest::updateWeightsPassAggr(std::vector<double> &scores, double *inst, int direction, double change, bool reg){
	for (std::vector<Tree*>::iterator it = this->trees.begin();
			it != trees.end(); ++it) {
		if(reg == true)
			(*it)->updateWeights(scores, inst, direction, 0, change);
		else
			(*it)->updateWeightsRunAvg(scores, inst, direction, change);
	}
}

void Forest::indexInstancesIntoNodes(const doubleframe* df){
	std::vector<int> initIdx;
	for(int i = 0; i < df->nrow; i++)
		initIdx.push_back(i);
	for (std::vector<Tree*>::iterator it = this->trees.begin();
			it != trees.end(); ++it) {
		(*it)->indexInstancesIntoNodes(initIdx, df);
	}
}

void Forest::computeMass(std::vector<double> &probScores){
	for (std::vector<Tree*>::iterator it = this->trees.begin();
			it != trees.end(); ++it) {
		(*it)->computeMass(probScores);
//		double res = (*it)->computeMass(probScores);
//		if(fabs(res-1) > 1e-15)
//			std::cout << "Error: " << res-1 << "\n";
	}
}

void Forest::writeScores(doubleframe *dt, char fName[]){
	unsigned int MAX = 20;
	std::ofstream outFile(fName);
	bool comma = false;
	for(int t = 0; t < this->ntree; ++t){
		for(unsigned int depLim = 1; depLim <= MAX; ++depLim){
			if(comma) outFile << ","; comma = true;
			outFile << "t" << t+1 << "d" << depLim;
		}
	}
	outFile << std::endl;
	for(int i = 0; i < dt->nrow; ++i){
		comma = false;
		for(int t = 0; t < this->ntree; ++t){
			std::vector<double> scores = this->trees[t]->getPatternScores(dt->data[i], MAX);
			for(unsigned int depLim = 0; depLim < MAX; ++depLim){
				if(comma) outFile << ","; comma = true;
				if(depLim < scores.size())
					outFile << scores[depLim];
			}
		}
		outFile << std::endl;
	}
	outFile.close();
}

void Forest::writeScoreDatabase(doubleframe *dtTestNorm, doubleframe *dtTestAnom, char fNamesuf[]){
	char fNameIF[100],fNamePA[100],fNamePM[100];
	double sumIF, sumPA, minPM;
	int depLim = -1;
	sprintf(fNameIF, "%s.IF.csv", fNamesuf);
	sprintf(fNamePA, "%s.PA.csv", fNamesuf);
	sprintf(fNamePM, "%s.PM.csv", fNamesuf);
	std::ofstream outIF(fNameIF), outPA(fNamePA), outPM(fNamePM);
	outIF << "groundtruth,score\n";
	outPA << "groundtruth,score\n";
	outPM << "groundtruth,score\n";
	for(int i = 0; i < dtTestNorm->nrow; ++i){
		sumIF = 0;
		sumPA = 0;
		minPM = 1e100;
		for(int t = 0; t < this->ntree; ++t){
			sumIF += this->trees[t]->getScoreAtDepth(dtTestNorm->data[i], depLim);
			std::vector<double> ps = this->trees[t]->getPatternScores(dtTestNorm->data[i], depLim);
			for(int j = 0; j < (int)ps.size(); ++j){
				sumPA += ps[j];
				if(ps[j] < minPM)
					minPM = ps[j];
			}
		}
		outIF << "nominal," << sumIF/this->ntree << std::endl;
		outPA << "nominal," << sumPA/this->ntree << std::endl;
		outPM << "nominal," << minPM << std::endl;
	}
	for(int i = 0; i < dtTestAnom->nrow; ++i){
		sumIF = 0;
		sumPA = 0;
		minPM = 1e100;
		for(int t = 0; t < this->ntree; ++t){
			sumIF += this->trees[t]->getScoreAtDepth(dtTestAnom->data[i], depLim);
			std::vector<double> ps = this->trees[t]->getPatternScores(dtTestAnom->data[i], depLim);
			for(int j = 0; j < (int)ps.size(); ++j){
				sumPA += ps[j];
				if(ps[j] < minPM)
					minPM = ps[j];
			}
		}
		outIF << "anomaly," << sumIF/this->ntree << std::endl;
		outPA << "anomaly," << sumPA/this->ntree << std::endl;
		outPM << "anomaly," << minPM << std::endl;
	}
	outIF.close();
	outPA.close();
	outPM.close();
}

std::vector<double> Forest::getScore(doubleframe *df, int type){
	std::vector<double> pscores[df->nrow];
	for(int i = 0; i < df->nrow; ++i){
		for(int t = 0; t < this->ntree; ++t){
			std::vector<double> psi = this->trees[t]->getPatternScores(df->data[i]);
			for(unsigned int j = 0; j < psi.size(); ++j)
				pscores[i].push_back(psi[j]);
		}
	}
	std::vector<double> res;
	double min, max, sum;
	for(int i = 0; i < df->nrow; ++i){
		min = max = sum = pscores[i][0];
		for(unsigned int j = 0; j < pscores[i].size(); ++j){
			if(min > pscores[i][j]) min = pscores[i][j];
			if(max < pscores[i][j]) max = pscores[i][j];
			sum += pscores[i][j];
		}
		if(type == 1)// minimum
			res.push_back(min);
		else if(type == 2)// mean
			res.push_back(sum/pscores[i].size());
		else if(type == 3){// median
			std::sort(pscores[i].begin(), pscores[i].end());
			res.push_back(pscores[i][pscores[i].size()/2]);
		}else// maximum
			res.push_back(max);
	}
	return res;
}

/*
 * Score for  a set of dataframe in dataset
 */
std::vector<double> Forest::AnomalyScore(const doubleframe* df, bool **marginalize) {
	std::vector<double> scores;
	//iterate through all points
	for (int inst = 0; inst < df->nrow; inst++) {
		scores.push_back(instanceMarginalScore(df->data[inst], marginalize));
	}
	return scores;
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
		bool rsample, int nrow, int *rndIdx) {
	if(nsample > nrow || nsample <= 0)
		nsample = nrow;

	for(int i = 0; i < nsample; ++i){
		int r = rand() % nrow;
		int t = rndIdx[i];
		rndIdx[i] = rndIdx[r];
		rndIdx[r] = t;
	}
	for(int i = 0; i < nsample; ++i){
		sampleIndex.push_back(rndIdx[i]);
	}
}

void Forest::printStat(std::ofstream &out){
	for(int i = 0; i < this->ntree; ++i){
		this->trees[i]->printDepthAndNodeSize(out);
//		break;
		out << "#####################################" << std::endl;
	}
}

void Forest::printPatternFreq(const doubleframe *df, int n, std::ofstream &out){
	out << "inst_id, tree_id, depth, nodes, volume, log_nodes_d_vol" << std::endl;
	for(int iid = 0; iid < n; iid++){
		for(int tid = 0; tid < this->ntree; tid++){
			this->trees[tid]->printPatternFreq(df->data[iid], tid, iid, out);
		}
	}
}
