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

int main(int argc, char* argv[]) {
	std::time_t st = std::time(nullptr);
	srand(0); //randomize for random number generator.
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
	int numFeedback = pargs->columns;//-c for train sample size option using for number of feedback
	if(numFeedback == 0)
		numFeedback = 100;
	int updateType = pargs->window_size;//-w for window_size option using as type of update
	if(updateType == 512)
		updateType = 0;

	char type[100];

	if(updateType == 0)
		strcpy(type, "reg");
	else if(updateType == 1)
		strcpy(type, "updMstk.reg");
	else if(updateType == 2)
		strcpy(type, "exp");
	else if(updateType == 3)
		strcpy(type, "updMstk.exp");
	else if(updateType == 4)
		strcpy(type, "runAvg");
	else if(updateType == 5)
		strcpy(type, "updMstk.runAvg");
	else if(updateType == 6)
		strcpy(type, "PA.runAvg");
	else if(updateType == 7)
		strcpy(type, "updMstk.PA.runAvg");
	else if(updateType == 8)
		strcpy(type, "PA.reg");
	else if(updateType == 9)
		strcpy(type, "updMstk.PA.reg");
	else if(updateType == 10)
		strcpy(type, "llh");
	else if(updateType == 11)
		strcpy(type, "llhWithReg");
	else if(updateType == 12)
		strcpy(type, "regWithReg");

	ntstringframe* csv = read_csv(input_name, header, false, false);
	ntstringframe* metadata = split_frame(ntstring, csv, metacol, true);
	doubleframe* dt = conv_frame(double, ntstring, csv); //read data to the global variable
	nsample = nsample == 0 ? dt->nrow : nsample;

	std::cout << "# Trees          = " << ntree << std::endl;
	std::cout << "# Samples        = " << nsample << std::endl;
	std::cout << "MaxHeight        = " << maxheight << std::endl;
	std::cout << "Orig Dimension   = " << dt->nrow << "," << dt->ncol << std::endl;
	std::cout << "# Feedbacks      = " << numFeedback << std::endl;
	std::cout << "Update type      = " << type << std::endl;

	int numIter = 10;
	double REG = 0.5;
//	char treeFile[1000];
//	char fname[1000];
	char statFile[1000], statNoFeed[1000];
//	sprintf(treeFile, "%s_tree_%s.txt", output_name, type);
	sprintf(statFile, "%s_summary_feed%d_type_%s.csv", output_name, numFeedback, type);
	sprintf(statNoFeed, "%s_summary_feed%d_type_%s.csv", output_name, 0, type);
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
		std::vector<bool> gotFeedback(dt->nrow, false);
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

			// Normalize scores to make an anomaly distribution
			double Z = 0, MN = scores[0];
			for(int i = 1; i < (int)scores.size(); i++){
				if(scores[i] < MN) MN = scores[i];
			}
			for(int i = 0; i < (int)scores.size(); i++){
				scoresNorm[i] = scores[i] - MN;
				Z += scoresNorm[i];
			}
			for(int i = 0; i < (int)scores.size(); i++)
				scoresNorm[i] = scoresNorm[i] / Z;
			iff.computeMass(scoresNorm);

			double min = DBL_MAX;
			int maxInd = -1;
			for(int i = 0; i < (int)scores.size(); i++){
				if(gotFeedback[i] == false && min > scores[i]){
					min = scores[i];
					maxInd = i;
				}
			}
//			std::cout << "maxIdx: " << maxInd << std::endl;
			gotFeedback[maxInd] = true;
			int direction = 1;
			if(strcmp(metadata->data[maxInd][0], "anomaly") == 0){
				direction = -1;
				numAnomFound++;
			}
			stats << "," << numAnomFound;

			if(updateType == 0){// regular weight update for both mistake and correct
				iff.updateWeights(scores, dt->data[maxInd], direction, 0);
			}
			else if(updateType == 1){
				if(direction < 0)//regular weight update only for false positives
					iff.updateWeights(scores, dt->data[maxInd], direction, 0);
			}
			else if(updateType == 2){// exponential weight update both for mistake and correct
				iff.updateWeights(scores, dt->data[maxInd], direction, 1);
			}
			else if(updateType == 3){
				if(direction < 0)//exponential weight update only for false positives
					iff.updateWeights(scores, dt->data[maxInd], direction, 1);
			}
			else if(updateType == 4){//running average weight update for both mistake and correct
				iff.updateWeightsRunAvg(scores, dt->data[maxInd], direction);
			}
			else if(updateType == 5){
				if(direction < 0)//running average weight update only for false positives
					iff.updateWeightsRunAvg(scores, dt->data[maxInd], direction);
			}
			else if(updateType == 6){//Passive Aggressive update with loss: max(0, \tau - y (w.x))
				double L2Norm2 = iff.getL2Norm2(dt->data[maxInd]);
				double threshold = fabs( getQthPercentileScore(scores, 0.03) );
				double loss = threshold - direction * scores[maxInd];
				if(loss > 0)
					iff.updateWeightsPassAggr(scores, dt->data[maxInd], direction, loss / L2Norm2, false);
			}
			else if(updateType == 7){//Passive Aggressive update with loss: max(0, \tau - y (w.x))
				double L2Norm2 = iff.getL2Norm2(dt->data[maxInd]);
				double threshold = fabs( getQthPercentileScore(scores, 0.03) );
				double loss = threshold - direction * scores[maxInd];
				if(loss > 0 && direction < 0)//update only on false positives
					iff.updateWeightsPassAggr(scores, dt->data[maxInd], direction, loss / L2Norm2, false);
			}
			else if(updateType == 8){//Passive Aggressive update with loss: max(0, \tau - y (w.x))
				double L2Norm2 = iff.getL2Norm2(dt->data[maxInd]);
				double threshold = fabs( getQthPercentileScore(scores, 0.03) );
				double loss = threshold - direction * scores[maxInd];
				if(loss > 0)
					iff.updateWeightsPassAggr(scores, dt->data[maxInd], direction, loss / L2Norm2, true);
			}
			else if(updateType == 9){//Passive Aggressive update with loss: max(0, \tau - y (w.x))
				double L2Norm2 = iff.getL2Norm2(dt->data[maxInd]);
				double threshold = fabs( getQthPercentileScore(scores, 0.03) );
				double loss = threshold - direction * scores[maxInd];
				if(loss > 0 && direction < 0)//update only on false positives
					iff.updateWeightsPassAggr(scores, dt->data[maxInd], direction, loss / L2Norm2, true);
			}
			else if(updateType == 10){// Update from gradient of-log likelihood loss
				iff.updateWeights(scores, dt->data[maxInd], direction, 1.0);
			}
			else if(updateType == 11){// Update from gradient of-log likelihood loss with regularization
				iff.updateWeights(scores, dt->data[maxInd], direction, 1.0, REG);
			}
			else if(updateType == 12){// regular weight update with regularization
				iff.updateWeights(scores, dt->data[maxInd], direction, 0, 1.0, REG);
			}
		}
		stats << "\n";
		statsNoFeed << "\n";
		std::cout << " Feedback -> " << numAnomFound << std::endl;
		sum += numAnomFound;
	}
	std::cout << "Avg: Baseline -> " << sumBase/numIter << " Feedback -> " << sum/numIter<< std::endl;
//	tree.close();
	stats.close();
	statsNoFeed.close();
	std::cout << "Time elapsed: " << std::time(nullptr) - st << " seconds\n";

	return 0;
}
