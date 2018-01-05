/*
 * main.cpp
 *
 @Created on: Mar 22, 2015
 * @Author: Tadeze
 * Main entry: accepts the the
 * @param argv
 *    Usage: iforest [options]
 *      -i FILE, --infile=FILE
 Specify path to input data file. (Required).
 -o FILE, --outfile=FILE
 Specify path to output results file. (Required).
 -m COLS, --metacol=COLS
 Specify columns to preserve as meta-data. (Separated by ',' Use '-' to specify ranges).
 -t N, --ntrees=N
 Specify number of trees to build.
 Default value is 100.
 -s S, --sampsize=S
 Specify subsampling rate for each tree. (Value of 0 indicates to use entire data set).
 Default value is 2048.
 -d MAX, --maxdepth=MAX
 Specify maximum depth of trees. (Value of 0 indicates no maximum).
 Default value is 0.
 -H, --header
 Toggle whether or not to expect a header input.
 Default value is true.
 -v, --verbose
 Toggle verbose ouput.
 Default value is false.
 -h, --help
 Print this help message and exit.
 */

#include "main.hpp"
#include<cfloat>
using namespace std;
//log file
//ofstream logfile("treepath.csv");

void deletedoubleframe(doubleframe *df) {
	for (int ii = 0; ii < df->nrow; ++ii) {
		delete[] df->data[ii];
	}
	delete[] df->data;
	delete df;
}

doubleframe *copyNormalInstances(const doubleframe *dtOrg, const ntstringframe* metadata) {
	int cntNormal = 0, cnt = 0;
	for(int i = 0; i < metadata->nrow; ++i){
		if(!strcmp(metadata->data[i][0], "nominal"))
			++cntNormal;
	}
	std::cout << "Number of normals: " << cntNormal << std::endl;
	doubleframe *dtOn = new doubleframe();
//	dtOn->colnames = dtOrg->colnames;
	dtOn->column_major = dtOrg->column_major;
	dtOn->ncol = dtOrg->ncol;
	dtOn->nrow = cntNormal;
//	dtOn->rownames = dtOrg->rownames;
	dtOn->data = new double *[dtOn->nrow];
	for (int ii = 0; ii < dtOn->nrow; ++ii) {
		dtOn->data[ii] = new double[dtOn->ncol];
	}
	for (int ii = 0; ii < metadata->nrow; ++ii) {
		if(!strcmp(metadata->data[ii][0], "nominal")){
			for (int jj = 0; jj < dtOn->ncol; ++jj) {
				dtOn->data[cnt][jj] = dtOrg->data[ii][jj];
			}
			++cnt;
		}
	}
	return dtOn;
}

doubleframe *copyAnomalyInstances(const doubleframe *dtOrg, const ntstringframe* metadata) {
	int cntAnomaly = 0, cnt = 0;
	for(int i = 0; i < metadata->nrow; ++i){
		if(!strcmp(metadata->data[i][0], "anomaly"))
			++cntAnomaly;
	}
	std::cout << "Number of anomaly: " << cntAnomaly << std::endl;
	doubleframe *dtOn = new doubleframe();
//	dtOn->colnames = dtOrg->colnames;
	dtOn->column_major = dtOrg->column_major;
	dtOn->ncol = dtOrg->ncol;
	dtOn->nrow = cntAnomaly;
//	dtOn->rownames = dtOrg->rownames;
	dtOn->data = new double *[dtOn->nrow];
	for (int ii = 0; ii < dtOn->nrow; ++ii) {
		dtOn->data[ii] = new double[dtOn->ncol];
	}
	for (int ii = 0; ii < metadata->nrow; ++ii) {
		if(!strcmp(metadata->data[ii][0], "anomaly")){
			for (int jj = 0; jj < dtOn->ncol; ++jj) {
				dtOn->data[cnt][jj] = dtOrg->data[ii][jj];
			}
			++cnt;
		}
	}
	return dtOn;
}

doubleframe *copySelectedRows(const doubleframe *dtOrg, std::vector<int> idx, int from, int to) {
	doubleframe *dtOn = new doubleframe();
//	dtOn->colnames = dtOrg->colnames;
	dtOn->column_major = dtOrg->column_major;
	dtOn->ncol = dtOrg->ncol;
	dtOn->nrow = to - from + 1;
//	dtOn->rownames = dtOrg->rownames;
	dtOn->data = new double *[dtOn->nrow];
	for (int ii = 0; ii < dtOn->nrow; ++ii) {
		dtOn->data[ii] = new double[dtOn->ncol];
	}
	for (int ii = 0; ii < dtOn->nrow; ++ii) {
		for (int jj = 0; jj < dtOn->ncol; ++jj) {
			dtOn->data[ii][jj] = dtOrg->data[idx[from + ii]][jj];
		}
	}
	return dtOn;
}


doubleframe *copyRows(const doubleframe *dtOrg, int from, int to) {
	doubleframe *dtOn = new doubleframe();
//	dtOn->colnames = dtOrg->colnames;
	dtOn->column_major = dtOrg->column_major;
	dtOn->ncol = dtOrg->ncol;
	dtOn->nrow = to - from + 1;
//	dtOn->rownames = dtOrg->rownames;
	dtOn->data = new double *[dtOn->nrow];
	for (int ii = 0; ii < dtOn->nrow; ++ii) {
		dtOn->data[ii] = new double[dtOn->ncol];
	}
	for (int ii = 0; ii < dtOn->nrow; ++ii) {
		for (int jj = 0; jj < dtOn->ncol; ++jj) {
			dtOn->data[ii][jj] = dtOrg->data[from + ii][jj];
		}
	}
	return dtOn;
}

doubleframe *copyCols(const doubleframe *dtOrg, int from, int to) {
	doubleframe *dtOn = new doubleframe();
//	dtOn->colnames = dtOrg->colnames;
	dtOn->column_major = dtOrg->column_major;
	dtOn->ncol = to - from + 1;
	dtOn->nrow = dtOrg->nrow;
//	dtOn->rownames = dtOrg->rownames;
	dtOn->data = new double *[dtOn->nrow];
	for (int ii = 0; ii < dtOn->nrow; ++ii) {
		dtOn->data[ii] = new double[dtOn->ncol];
	}
	for (int ii = 0; ii < dtOn->nrow; ++ii) {
		for (int jj = 0; jj < dtOn->ncol; ++jj) {
			dtOn->data[ii][jj] = dtOrg->data[ii][from + jj];
		}
	}
	return dtOn;
}

std::vector<int> getRandomIdx(int nsample, int nrow) {
	std::vector<int> sampleIndex;
	if(nsample > nrow || nsample <= 0)
		nsample = nrow;
	int *rndIdx = new int[nrow];
	for(int i = 0; i < nrow; ++i)
		rndIdx[i] = i;
	for(int i = 0; i < nsample; ++i){
		int r = std::rand() % nrow;
		int t = rndIdx[i];
		rndIdx[i] = rndIdx[r];
		rndIdx[r] = t;
	}
	for(int i = 0; i < nsample; ++i){
		sampleIndex.push_back(rndIdx[i]);
	}
	delete []rndIdx;
	return sampleIndex;
}

doubleframe *createTrainingSet(doubleframe *dtTrainNorm, doubleframe *dtTrainAnom, int numNorm, int numAnom){
	doubleframe *dtOn = new doubleframe();

	dtOn->column_major = dtTrainNorm->column_major;
	dtOn->ncol = dtTrainNorm->ncol;
	dtOn->nrow = numNorm + numAnom;

	dtOn->data = new double *[dtOn->nrow];
	for (int ii = 0; ii < dtOn->nrow; ++ii) {
		dtOn->data[ii] = new double[dtOn->ncol];
	}
	std::vector<int> nidx = getRandomIdx(numNorm, dtTrainNorm->nrow);
	std::vector<int> aidx = getRandomIdx(numAnom, dtTrainAnom->nrow);
	int i = 0, j = 0;
	for (int ii = 0; ii < dtOn->nrow; ++ii) {
		int normal = std::rand() % (numNorm + numAnom);
		if(normal >= numAnom && i >= (int)nidx.size())
			normal = 0;
		if(normal < numAnom && j >= (int)aidx.size())
			normal = numAnom;
		for (int jj = 0; jj < dtOn->ncol; ++jj) {
			if(normal >= numAnom)
				dtOn->data[ii][jj] = dtTrainNorm->data[nidx[i]][jj];
			else
				dtOn->data[ii][jj] = dtTrainAnom->data[aidx[j]][jj];
		}
		if(normal >= numAnom) ++i;
		else ++j;
	}
	return dtOn;
}

std::vector<int> getRestIdx(std::vector<int> idx, int nrow){
	std::vector<int> restidx;
	bool track[nrow];
	for(int i = 0; i < nrow; ++i)
		track[i] = true;
	for(int i = 0; i < (int)idx.size(); ++i)
		track[idx[i]] = false;
	for(int i = 0; i < nrow; ++i){
		if(track[i] == true)
			restidx.push_back(i);
	}
	return restidx;
}

int sidx = 0;
struct sItem {
	double *p;
	ntstring *q;
};
int cmp(sItem *a, sItem *b) {
	if (a->p[sidx] > b->p[sidx])
		return 1;
	if (a->p[sidx] < b->p[sidx])
		return -1;
	return 0;
}

void printData(int fid, doubleframe* dt, ntstringframe* metadata) {
	char fname[100];
	sprintf(fname, "data%d.csv", fid + 1);
	std::ofstream out(fname);
	for (int i = 0; i < dt->nrow; ++i) {
		if(metadata)
			out << metadata->data[i][0];
		for (int j = 0; j < dt->ncol; ++j) {
			out << "," << dt->data[i][j];
		}
		out << std::endl;
	}
	out.close();
}
void sortByFeature(int fid, doubleframe* dt, ntstringframe* metadata) {
	sidx = fid;
	sItem *tp = new sItem[dt->nrow];
	for (int i = 0; i < dt->nrow; ++i) {
		tp[i].p = dt->data[i];
		tp[i].q = metadata->data[i];
	}
	qsort(tp, dt->nrow, sizeof(tp[0]),
			(int (*)(const void *, const void *))cmp);
	for(int i = 0; i < dt->nrow; ++i) {
		dt->data[i] = tp[i].p;
		metadata->data[i] = tp[i].q;
	}
	delete[] tp;
}

// randomly shuffle dt along with metadata
void randomShuffle(doubleframe* dt, ntstringframe* metadata) {
	for (int ii = 0; ii < dt->nrow; ++ii) {
		int ridx = rand() % dt->nrow;
		if (metadata) {
			ntstring *t = metadata->data[ii];
			metadata->data[ii] = metadata->data[ridx];
			metadata->data[ridx] = t;
		}
		double *t = dt->data[ii];
		dt->data[ii] = dt->data[ridx];
		dt->data[ridx] = t;
	}
}

void printScoreToFile(vector<double> &scores, const ntstringframe* metadata,
		char fName[]) {
	ofstream outscore;
	outscore.open(fName);
	outscore << "groundtruth,score\n";
	for (int j = 0; j < (int) scores.size(); j++) {
		if (metadata) {
			outscore << metadata->data[j][0] << ",";
		}
		outscore << scores[j] << "\n";
	}
	outscore.close();
}

struct Obj{
    int idx;
    double score;
};
int ObjCmp(Obj *a, Obj *b){
    if(a->score > b->score) return 1;
    if(a->score < b->score) return -1;
    return 0;
}

int printNoFeedbackAnomCntToFile(const vector<double> &scores, const ntstringframe* metadata,
		ofstream &outscore, int numFeed) {
    Obj *idx = new Obj[scores.size()];
	for (int i = 0; i < (int) scores.size(); i++) {
        idx[i].idx = i;
        idx[i].score = scores[i];
    }
    qsort(idx, scores.size(), sizeof(idx[0]),
			(int (*)(const void *, const void *))ObjCmp);

    int numAnom = 0;
	for (int i = 0; i < numFeed; i++) {
		if(strcmp(metadata->data[idx[i].idx][0], "anomaly") == 0)
			numAnom++;
		outscore << "," << numAnom;
	}
	delete []idx;
	return numAnom;
}


void printScoreToFile(const vector<double> &scores, ntstringframe* csv,
		const ntstringframe* metadata, const doubleframe *dt, char fName[]) {
	ofstream outscore;
	outscore.open(fName);
	// print header
	for(int i = 0; i < metadata->ncol; ++i)
		outscore << metadata->colnames[i] << ",";
	for(int i = 0; i < csv->ncol; ++i)
        outscore << csv->colnames[i] << ",";

	outscore << "anomaly_score\n";

    Obj *idx = new Obj[scores.size()];
	for (int i = 0; i < (int) scores.size(); i++) {
        idx[i].idx = i;
        idx[i].score = scores[i];
    }
    qsort(idx, scores.size(), sizeof(idx[0]),
			(int (*)(const void *, const void *))ObjCmp);

	for (int i = 0; i < (int) scores.size(); i++) {
		for(int j = 0; j < metadata->ncol; ++j)
			outscore << metadata->data[idx[i].idx][j] << ",";
		for(int j = 0; j < dt->ncol; ++j)
			outscore << dt->data[idx[i].idx][j] << ",";
		outscore << scores[idx[i].idx] << "\n";
	}
	delete []idx;
	outscore.close();
}

void explanationFeedback(doubleframe* dt, ntstringframe* metadata,
		IsolationForest &iff, std::vector<double> scores, int iter, char type[], char out_name[], int numfeed){
	char fname[100];
	int **freq = new int *[dt->ncol];
	bool **marginalize = new bool *[dt->ncol];
	for(int i = 0; i < dt->ncol; i++){
		freq[i]	= new int[4];
		marginalize[i] = new bool[4];
		for(int j = 0; j < 4; j++)
			marginalize[i][j] = false;
	}

	for(int rep = 0; rep < numfeed; rep++){
		for(int i = 0; i < dt->ncol; i++)
			for(int j = 0; j < 4; j++)
				freq[i][j] = 0;

		for(int top = 0; top < 1; top++){
			double max = -1;
			int midx = -1;
			for(int i = 0; i < (int)scores.size(); i++){
				if(scores[i] > max && strcmp(metadata->data[i][0], "nominal") == 0){
					max = scores[i];
					midx = i;
				}
			}

			scores[midx] = -1;

			std::vector<int> expl;
			if(strcmp(type,"seq_marg") == 0)
				expl = iff.getSeqMarExplanation(dt->data[midx], dt->ncol, marginalize, 1);
			else if(strcmp(type,"seq_drop") == 0)
				expl = iff.getSeqDropExplanation(dt->data[midx], dt->ncol, marginalize, 1);
			else if(strcmp(type,"rev_seq_marg") == 0)
				expl = iff.getRevSeqMarExplanation(dt->data[midx], dt->ncol, marginalize);
			else if(strcmp(type,"rev_seq_drop") == 0)
				expl = iff.getRevSeqDropExplanation(dt->data[midx], dt->ncol, marginalize);

			// Disregard this instance if no explanation found
			if(expl.size() == 0) continue;

			int tf = expl[0];
			int q = Tree::getQuantile(tf, dt->data[midx][tf]);
			freq[tf][q]++;
		}

		int marg_feat = -1, q = -1;
		int max = 0;
		for(int i = 0; i < dt->ncol; i++){
			for(int j = 0; j < 4; j++){
				if(freq[i][j] > max){
					max = freq[i][j];
					marg_feat = i;
					q = j;
				}
			}
		}

		// stop if no feedback to give
		if(marg_feat < 0 || q < 0) break;

		marginalize[marg_feat][q] = true;
		scores = iff.AnomalyScore(dt, marginalize);
		std::cout <<"Feedback " << rep+1 << ": Removed " << marg_feat << " " << q << "\n" << std::flush;
		sprintf(fname, "%s_%s_iter%d_%d.csv", out_name, type, iter+1, rep+1);
		printScoreToFile(scores, metadata, fname);
	}

	std::cout << "\n";

	for(int i = 0; i < dt->ncol; i++){
		delete []freq[i];
		delete []marginalize[i];
	}
	delete []freq;
	delete []marginalize;
}

double getQthPercentileScore(const std::vector<double> &scores, double q){
	double *x = new double[scores.size()];
	for(int i = 0; i < (int)scores.size(); i++)
		x[i] = scores[i];
	qsort(x, sizeof(x[0]), scores.size(), (int (*)(const void *, const void *))dblcmp);
	int k = (int)ceil(scores.size()*q);
	double quant = x[scores.size()-k];
	delete []x;
	return quant;
}

// Normalize scores to make an anomaly distribution
void normalizeScore(std::vector<double> &scores, std::vector<double> &scoresNorm){
	double Z = 0, MIN = scores[0];
	for(int i = 1; i < (int)scores.size(); i++){
		if(scores[i] < MIN) MIN = scores[i];
	}
	for(int i = 0; i < (int)scores.size(); i++){
		scoresNorm[i] = exp(MIN - scores[i]);
		Z += scoresNorm[i];
	}
	for(int i = 0; i < (int)scores.size(); i++)
		scoresNorm[i] = scoresNorm[i] / Z;
}

double getLinearLoss(int x, int y, std::vector<double> &scores){
	double loss = y * scores[x];
	return loss;
}
double getLinearLoss(std::vector<int> x, const ntstringframe* metadata, std::vector<double> &scores){
	double loss = 0;
	for(int i = 0; i < (int)x.size(); i++){
		int y = (strcmp(metadata->data[x[i]][0], "anomaly") == 0) ? 1 : -1;
		loss += getLinearLoss(x[i], y, scores);
	}
	loss /= x.size();
	return loss;
}


double getLoglikelihoodLoss(int x, int y, std::vector<double> &scoresNorm){
	double loss = (-y) * log(scoresNorm[x]+1e-15);
	return loss;
}

double getLoglikelihoodLoss(std::vector<int> x, const ntstringframe* metadata, std::vector<double> &scoresNorm){
	double loss = 0;
	for(int i = 0; i < (int)x.size(); i++){
		int y = (strcmp(metadata->data[x[i]][0], "anomaly") == 0) ? 1 : -1;
		loss += getLoglikelihoodLoss(x[i], y, scoresNorm);
	}
	loss /= x.size();
	return loss;
}

double getLogisticLoss(int x, int y, std::vector<double> &scores){
	double loss = y * scores[x];
	if(loss > 100)
		return loss;
	else
		return log(1 + exp(loss));
}

double getLogisticLoss(std::vector<int> x, const ntstringframe* metadata, std::vector<double> &scores){
	double loss = 0;
	for(int i = 0; i < (int)x.size(); i++){
		int y = (strcmp(metadata->data[x[i]][0], "anomaly") == 0) ? 1 : -1;
		loss += getLogisticLoss(x[i], y, scores);
	}
	loss /= x.size();
	return loss;
}

std::vector<int> rndVec[100];// need to allocate memory if feedback greater than 100
void initRndVec(){
	for(int s = 1; s <= 100; s++){
		for(int i = 0; i < s; i++){
			rndVec[s-1].push_back(rand() % s);
		}
	}
}
void shuffle(std::vector<int> &feedbackIdx){
	for(int j = 0; j < (int)feedbackIdx.size(); j++){
		int rnd = rndVec[feedbackIdx.size()-1][j];
		int tmp = feedbackIdx[j];
		feedbackIdx[j] = feedbackIdx[rnd];
		feedbackIdx[rnd] = tmp;
	}
}

int main(int argc, char* argv[]) {
	std::time_t st = std::time(nullptr);
	srand(0); //randomize for random number generator.
	initRndVec();
	// parse argument from command line
	parsed_args* pargs = parse_args(argc, argv);
	ntstring input_name = pargs->input_name;
	ntstring output_name = pargs->output_name;
	d(int)* metacol = pargs->metacol;
	int ntree = pargs->ntrees;
	int nsample = pargs->sampsize;
	int maxheight = pargs->maxdepth;
	bool header = pargs->header;
//	bool verbose = pargs->verbose;
	bool rsample = nsample != 0;

	// Feedback Parameters
	int numIter = pargs->numIteration;
	int numFeedback = pargs->numFeedback;
	int numGradUpd = pargs->numGradUpd;
	int lossType = pargs->lossType;
	int updateType = pargs->updateType;
	double REG = pargs->REG_PARAM;

	char typeLoss[100], typeUpdate[100];
	if(lossType == 0)
		strcpy(typeLoss, "linear");
	else if(lossType == 1)
		strcpy(typeLoss, "loglikelihood");
	else if(lossType == 2)
		strcpy(typeLoss, "logistic");

	if(updateType == 0)
		strcpy(typeUpdate, "online");
	else if(updateType == 1)
		strcpy(typeUpdate, "stochastic");
	else if(updateType == 2)
		strcpy(typeUpdate, "batch");

	ntstringframe* csv = read_csv(input_name, header, false, false);
	ntstringframe* metadata = split_frame(ntstring, csv, metacol, true);
	doubleframe* dt = conv_frame(double, ntstring, csv); //read data to the global variable
	nsample = nsample == 0 ? dt->nrow : nsample;

	std::cout << "# Trees          = " << ntree << std::endl;
	std::cout << "# Samples        = " << nsample << std::endl;
	std::cout << "MaxHeight        = " << maxheight << std::endl;
	std::cout << "Orig Dimension   = " << dt->nrow << "," << dt->ncol << std::endl;
	std::cout << "# Iterations     = " << numIter << std::endl;
	std::cout << "# Feedbacks      = " << numFeedback << std::endl;
	std::cout << "Loss   type      = " << typeLoss << std::endl;
	std::cout << "Update type      = " << typeUpdate << std::endl;
	std::cout << "Num Grad Upd     = " << numGradUpd << std::endl;
	std::cout << "Reg. Constant    = " << REG << std::endl;

//	char treeFile[1000];
//	char fname[1000];
	char statFile[1000], statNoFeed[1000];
	double costBefore[100], costAfter[100], avgcostBefore[100], avgcostAfter[100];//need dynamic memory allocation
//	sprintf(treeFile, "%s_tree_%s.txt", output_name, type);
	sprintf(statFile,   "%s_summary_feed_%d_losstype_%s_updatetype_%s_ngrad_%d_reg_%g.csv",
			output_name, numFeedback, typeLoss, typeUpdate, numGradUpd, REG);
	sprintf(statNoFeed, "%s_summary_feed_%d_losstype_%s_updatetype_%s_ngrad_%d_reg_%g.csv",
			output_name,           0, typeLoss, typeUpdate, numGradUpd, REG);
//	ofstream tree(treeFile);
	ofstream stats(statFile), statsNoFeed(statNoFeed);
	stats << "iter";
	statsNoFeed << "iter";
	for(int i = 0; i < numFeedback; i++){
		stats << ",feed" << i+1;
		statsNoFeed << ",feed" << i+1;
	}
	stats << "\n";
	statsNoFeed << "\n";

	char costFile[1000];
	sprintf(costFile, "%s_cost_feed_%d_losstype_%s_updatetype_%s_ngrad_%d_reg_%g.csv",
			output_name, numFeedback, typeLoss, typeUpdate, numGradUpd, REG);
	ofstream costs(costFile);
	costs << "iter";
	for(int i = 0; i < numFeedback; i++){
		costs << ",feed" << i+1;
	}
	costs << "\n";

	double sum = 0, sumBase = 0;
	for(int iter = 0; iter < numIter; iter++){
		stats << iter+1;
		statsNoFeed << iter+1;
		int numAnomFound = 0;
		std::cout << "iter " << iter << ", # Anomaly: ";
		IsolationForest iff(ntree, dt, nsample, maxheight, rsample);
		iff.indexInstancesIntoNodes(dt);
		std::vector<double> scores(dt->nrow, 0.0), scoresNorm(dt->nrow, 0.0);
		iff.weightIndexedScore(scores);
		normalizeScore(scores, scoresNorm);
		iff.computeMass(scoresNorm);
		std::vector<bool> gotFeedback(dt->nrow, false);
		std::vector<int> feedbackIdx;
		for(int feed = 0; feed < numFeedback; feed++){
			if(feed == 0){
				int baseAnom = printNoFeedbackAnomCntToFile(scores, metadata, statsNoFeed, numFeedback);
				std::cout << "Baseline -> " << baseAnom;
				sumBase += baseAnom;
			}
//			if(feed == 0 || feed == (numFeedback-1)){
//				iff.printStat(tree);
//			}
//			if(feed == 0 || feed == (numFeedback-1)){
//				sprintf(fname, "%s_iter%d_feed%d_type_%s.csv", output_name, iter+1, feed, type);
//				printScoreToFile(scores, csv, metadata, dt, fname);
//			}

			double min = DBL_MAX;
			int maxInd = -1;
			for(int i = 0; i < (int)scores.size(); i++){
				if(gotFeedback[i] == false && min > scores[i]){
					min = scores[i];
					maxInd = i;
				}
			}
			gotFeedback[maxInd] = true;
			feedbackIdx.push_back(maxInd);
			int y = -1;
			if(strcmp(metadata->data[maxInd][0], "anomaly") == 0){
				y = 1;
				numAnomFound++;
			}
			stats << "," << numAnomFound;
			if(lossType == 0){//linear loss
				costBefore[feed] = getLinearLoss(maxInd, y, scores);
				avgcostBefore[feed] = getLinearLoss(feedbackIdx, metadata, scores);

				if(updateType == 0){// online update
					for(int i = 0; i < numGradUpd; i++)
						iff.updateWeights(scores, dt->data[maxInd], -y, 0, 1.0, REG);
				}
				else if(updateType == 1){// stochastic update
					for(int i = 0; i < numGradUpd; i++){
						shuffle(feedbackIdx);
						for(int j = 0; j < (int)feedbackIdx.size(); j++){
							iff.updateWeights(scores, dt->data[feedbackIdx[j]], -y, 0, 1.0, REG);
						}
					}
				}
				else if(updateType == 2){// batch update
					for(int i = 0; i < numGradUpd; i++){
						for(int j = 0; j < (int)feedbackIdx.size()-1; j++){
							iff.updateWeights(scores, dt->data[feedbackIdx[j]], -y, 0, 1.0/feedbackIdx.size(), 0);
						}
						iff.updateWeights(scores, dt->data[feedbackIdx[feedbackIdx.size()-1]], -y, 0, 1.0/feedbackIdx.size(), REG);
					}
				}

				costAfter[feed] = getLinearLoss(maxInd, y, scores);
				avgcostAfter[feed] = getLinearLoss(feedbackIdx, metadata, scores);
			}
			else if(lossType == 1){//log likelihood loss
				costBefore[feed] = getLoglikelihoodLoss(maxInd, y, scoresNorm);
				avgcostBefore[feed] = getLoglikelihoodLoss(feedbackIdx, metadata, scoresNorm);

				if(updateType == 0){// online update
					for(int i = 0; i < numGradUpd; i++){
						iff.updateWeights(scores, dt->data[maxInd], -y, 1.0, REG);
						normalizeScore(scores, scoresNorm);
						iff.computeMass(scoresNorm);
					}
				}
				else if(updateType == 1){// stochastic update
					for(int i = 0; i < numGradUpd; i++){
						shuffle(feedbackIdx);
						for(int j = 0; j < (int)feedbackIdx.size(); j++){
							iff.updateWeights(scores, dt->data[feedbackIdx[j]], -y, 1.0, REG);
							normalizeScore(scores, scoresNorm);
							iff.computeMass(scoresNorm);
						}
					}
				}
				else if(updateType == 2){// batch update
					for(int i = 0; i < numGradUpd; i++){
						for(int j = 0; j < (int)feedbackIdx.size()-1; j++){
							iff.updateWeights(scores, dt->data[feedbackIdx[j]], -y, 1.0/feedbackIdx.size(), 0);
						}
						iff.updateWeights(scores, dt->data[feedbackIdx[feedbackIdx.size()-1]], -y, 1.0/feedbackIdx.size(), REG);
						normalizeScore(scores, scoresNorm);
						iff.computeMass(scoresNorm);
					}
				}

				costAfter[feed] = getLoglikelihoodLoss(maxInd, y, scoresNorm);
				avgcostAfter[feed] = getLoglikelihoodLoss(feedbackIdx, metadata, scoresNorm);
			}
			else if(lossType == 2){//logistic loss
				costBefore[feed] = getLogisticLoss(maxInd, y, scores);
				avgcostBefore[feed] = getLogisticLoss(feedbackIdx, metadata, scores);

				if(updateType == 0){// online update
					for(int i = 0; i < numGradUpd; i++){
						double change = 1 / (1 + exp(-y * scores[maxInd]));
						if(change > 1e-6){
							iff.updateWeights(scores, dt->data[maxInd], -y, 0, change, REG);
						}
					}
				}
				else if(updateType == 1){// stochastic update
					for(int i = 0; i < numGradUpd; i++){
						shuffle(feedbackIdx);
						for(int j = 0; j < (int)feedbackIdx.size(); j++){
							double change = 1 / (1 + exp(-y * scores[feedbackIdx[j]]));
							if(change > 1e-6){
								iff.updateWeights(scores, dt->data[feedbackIdx[j]], -y, 0, change, REG);
							}
						}
					}
				}
				else if(updateType == 2){// batch update
					for(int i = 0; i < numGradUpd; i++){
						for(int j = 0; j < (int)feedbackIdx.size()-1; j++){
							double change = 1 / (1 + exp(-y * scores[feedbackIdx[j]]));
							if(change > 1e-6){
								iff.updateWeights(scores, dt->data[feedbackIdx[j]], -y, 0, change/feedbackIdx.size(), 0);
							}
						}
						double change = 1 / (1 + exp(-y * scores[feedbackIdx[feedbackIdx.size()-1]]));
						iff.updateWeights(scores, dt->data[feedbackIdx[feedbackIdx.size()-1]], -y, 0, change/feedbackIdx.size(), REG);
					}
				}

				costAfter[feed] = getLogisticLoss(maxInd, y, scores);
				avgcostAfter[feed] = getLogisticLoss(feedbackIdx, metadata, scores);
			}
		}
		stats << "\n";
		statsNoFeed << "\n";
		std::cout << " Feedback -> " << numAnomFound << std::endl;
		sum += numAnomFound;

		costs << iter+1;
		for(int feed = 0; feed < numFeedback; feed++)
			costs << "," << costBefore[feed];
		costs << "\n";
		costs << iter+1;
		for(int feed = 0; feed < numFeedback; feed++)
			costs << "," << costAfter[feed];
		costs << "\n";
		costs << iter+1;
		for(int feed = 0; feed < numFeedback; feed++)
			costs << "," << avgcostBefore[feed];
		costs << "\n";
		costs << iter+1;
		for(int feed = 0; feed < numFeedback; feed++)
			costs << "," << avgcostAfter[feed];
		costs << "\n";
	}
	std::cout << "Avg: Baseline -> " << sumBase/numIter << " Feedback -> " << sum/numIter<< std::endl;
//	tree.close();
	stats.close();
	statsNoFeed.close();
	costs.close();
	std::cout << "Time elapsed: " << std::time(nullptr) - st << " seconds\n";

	return 0;
}
