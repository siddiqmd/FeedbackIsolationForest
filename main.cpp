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
			(int (*)(const void *, const void *))cmp);for
(	int i = 0; i < dt->nrow; ++i) {
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

int main(int argc, char* argv[]) {
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
	int useColumns = pargs->columns;
//	std::cout << useColumns << std::endl;
//	int windowSize = pargs->window_size;

	ntstringframe* csv = read_csv(input_name, header, false, false);
	ntstringframe* metadata = split_frame(ntstring, csv, metacol, true);
	doubleframe* dt = conv_frame(double, ntstring, csv); //read data to the global variable
	nsample = nsample == 0 ? dt->nrow : nsample;

	std::cout << "# Trees     = " << ntree << std::endl;
	std::cout << "# Samples   = " << nsample << std::endl;
	std::cout << "# MaxHeight = " << maxheight << std::endl;
	std::cout << "Original Data Dimension: " << dt->nrow << "," << dt->ncol
			<< std::endl;

//	if(1 == 1){
//		printData(-1, dt, metadata);
//		for(int ii = 0; ii < dt->ncol; ++ii){
//			sortByFeature(ii, dt, metadata);
//			std::cout << ii << std::endl;
//			printData(ii, dt, metadata);
//		}
//		return 0;
//	}
	if (useColumns > 0 && dt->ncol > useColumns) {
		doubleframe* temp = copyCols(dt, 0, useColumns - 1);
		dt = temp;
		std::cout << "Using columns: " << dt->ncol << std::endl;
	}
	char fName[100];
//	std::cout << dt->nrow << " " << dt->ncol << std::endl;
//	vector<int> iforest, pattern;
//	int maxInstId = 0;
//	for(int i = 0; i < dt->nrow; ++i){
//		if(strcmp(metadata->data[i][0], "I") == 0)
//			iforest.push_back(i);
//		else
//			pattern.push_back(i);
//		if(dt->data[i][0] > maxInstId)
//			maxInstId = (int)dt->data[i][0];
//	}
//	{
//	vector<int> ifdepth[10];
//	for(int i = 0; i < (int)iforest.size(); ++i){
//		ifdepth[(int)dt->data[iforest[i]][2]-1].push_back(iforest[i]);
//	}
//	std::cout << "maxInstId = " << maxInstId << std::endl;
//	for(int d = 0; d < 10; d++){
////		std::cout << d << std::endl;
//		vector<int> instId[maxInstId];
//		for(int i = 0; i < (int)ifdepth[d].size(); ++i){
//			instId[(int)(dt->data[ifdepth[d][i]][0])-1].push_back(ifdepth[d][i]);
//		}
//		int tt = strlen(pargs->input_name)-1;
//		while(pargs->input_name[tt] != '/' && pargs->input_name[tt] !='\\')
//			--tt;
//		sprintf(fName, "IF.d%d.%s", d+1, &pargs->input_name[tt+1]);
////		std::cout << fName << endl;
//		std::ofstream out(fName);
//		out << "groundtruth,score" << endl;
//		for(int i = 0; i < maxInstId; ++i){
//			double sum = 0;
//			for(int j = 0; j < (int)instId[i].size(); ++j){
//				sum += dt->data[instId[i][j]][3];
//			}
//			out << metadata->data[ instId[i][0] ][1]  << "," << sum/instId[i].size() << std::endl;
//		}
//		out.close();
//	}
//	}
//	{
//	vector<int> ifdepth[10];
//	for(int i = 0; i < (int)pattern.size(); ++i){
//		ifdepth[(int)dt->data[pattern[i]][2]-1].push_back(pattern[i]);
//	}
//	std::cout << "maxInstId = " << maxInstId << std::endl;
//	for(int d = 0; d < 10; d++){
////		std::cout << d << std::endl;
//		vector<int> instId[maxInstId];
//		for(int i = 0; i < (int)ifdepth[d].size(); ++i){
//			instId[(int)(dt->data[ifdepth[d][i]][0])-1].push_back(ifdepth[d][i]);
//		}
//		int tt = strlen(pargs->input_name)-1;
//		while(pargs->input_name[tt] !='/' && pargs->input_name[tt] != '\\')
//			--tt;
//		sprintf(fName, "PA.d%d.%s", d+1, &pargs->input_name[tt+1]);
////		std::cout << fName << endl;
//		std::ofstream out(fName);
//		out << "groundtruth,score" << endl;
//		for(int i = 0; i < maxInstId; ++i){
//			double sum = 0;
//			for(int j = 0; j < (int)instId[i].size(); ++j){
//				sum += dt->data[instId[i][j]][3];
//			}
//			out << metadata->data[ instId[i][0] ][1]  << "," << sum/instId[i].size() << std::endl;
//		}
//		out.close();
//	}
//	}
//	if(1 == 1) return 0;
//	for(int i = 0; i < 5; i++){
//		std::vector<int> idx = getRandomIdx(5, 10);
//		for(int j = 0; j < (int)idx.size(); j++)
//			std::cout << idx[j] << " ";
//		std::cout << "\n";
//	}

	// Code for generating learning curve data
	std::cout << "CheckRange = " << Tree::checkRange << std::endl;
	std::cout << "Volume = " << Tree::useVolumeForScore << std::endl;
	doubleframe *dtNorm = copyNormalInstances(dt, metadata);
	doubleframe *dtAnom = copyAnomalyInstances(dt, metadata);
	std::vector<int> nidx = getRandomIdx((int)std::ceil(dtNorm->nrow * 0.2), dtNorm->nrow);
	std::vector<int> aidx = getRandomIdx((int)std::ceil(dtAnom->nrow * 0.2), dtAnom->nrow);
	doubleframe *dtTestNorm = copySelectedRows(dtNorm, nidx, 0, nidx.size()-1);
	doubleframe *dtTestAnom = copySelectedRows(dtAnom, aidx, 0, aidx.size()-1);
	std::cout << "# Test normals = " << dtTestNorm->nrow << std::endl;
	std::cout << "# Test anomaly = " << dtTestAnom->nrow << std::endl;
	std::vector<int> restnidx, restaidx;
	bool track1[dtNorm->nrow], track2[dtAnom->nrow];
	for(int i = 0; i < dtNorm->nrow; i++)
		track1[i] = true;
	for(int i = 0; i < (int)nidx.size(); ++i)
		track1[nidx[i]] = false;
	for(int i = 0; i < dtNorm->nrow; ++i){
		if( track1[i] == true)
			restnidx.push_back(i);
	}
	for(int i = 0; i < dtAnom->nrow; i++)
		track2[i] = true;
	for(int i = 0; i < (int)aidx.size(); ++i)
		track2[aidx[i]] = false;
	for(int i = 0; i < dtAnom->nrow; ++i){
		if( track2[i] == true)
			restaidx.push_back(i);
	}
	doubleframe *dtTrainNorm = copySelectedRows(dtNorm, restnidx, 0, restnidx.size()-1);
	doubleframe *dtTrainAnom = copySelectedRows(dtAnom, restaidx, 0, restaidx.size()-1);
	std::cout << "# Train normals = " << dtTrainNorm->nrow << std::endl;
	std::cout << "# Train anomaly = " << dtTrainAnom->nrow << std::endl;
	deletedoubleframe(dtNorm);
	deletedoubleframe(dtAnom);

	double total = (dtTrainNorm->nrow + dtTrainAnom->nrow);
	for(int s = 16; s < total; s *= 2){
		std::cout << "s = " << s << std::endl;
		int numNorm = (int)floor(s * dtTrainNorm->nrow / total);
		int numAnom = (int)ceil(s * dtTrainAnom->nrow / total);
		if(numNorm + numAnom > s)
			numNorm--;
		else if(numNorm + numAnom < s)
			numNorm++;
		std::cout << "# normals = " << numNorm << std::endl;
		std::cout << "# anomaly = " << numAnom << std::endl;
		if(numNorm + numAnom != s)
			std::cout << "Error 1" << std::endl;
		for (int rep = 0; rep < 10; ++rep) {
			std::cout << "rep = " << rep << std::endl;
			doubleframe *trainSet = createTrainingSet(dtTrainNorm, dtTrainAnom, numNorm, numAnom);
			sprintf(fName, "%s.%d.%d", output_name, s, rep);
//			ofstream out(fName);
//			for (int i = 0; i < trainSet->nrow; ++i) {
//				for (int j = 0; j < trainSet->ncol; ++j) {
//					out << "," << trainSet->data[i][j];
//				}
//				out << std::endl;
//			}
//			out << std::endl;
//			out.close();
			IsolationForest iff(ntree, trainSet, nsample, maxheight, rsample);
			iff.writeScoreDatabase(dtTestNorm, dtTestAnom, fName);
			deletedoubleframe(trainSet);
		}
	}


//	int WINSIZE[] = { 128, 256, 512, 1024, 2048 };
//	char fName[100];
//	vector<double> scores;//, scoresMin, scoresAvg;
//	int windowSize;
//
////	doubleframe *normalData = copyNormalInstances(dt, metadata);
//
//	for(int checkrange = 0; checkrange <= 0; ++checkrange){
//		Tree::checkRange = (checkrange == 1);
////		std::cout << "CheckRange = " << checkrange << std::endl;
//		for (int v = 0; v <= 0; ++v) {
//			Tree::useVolumeForScore = (v == 1);
////			std::cout << "Volume = " << v << std::endl;
//			for (int rep = 0; rep < 1; ++rep) {
//				std::cout << "Rep = " << rep << std::endl;
//
//				// standard IsolationForest
//				IsolationForest iff(ntree, dt, nsample, maxheight, rsample);
//
////				sprintf(fName, "%s.stat.v%d.r%d.c%d.csv", output_name,
////						v, rep, checkrange);
////				ofstream outtree(fName);
////				iff.printPatternFreq(dt, 100, outtree);
////				outtree.close();
////				if(1 == 1) return 0;
//
////				sprintf(fName, "D:/ADAPT/OIF/Test/LeastFrequentPattern/tree.v%d.r%d.c%d.txt",
////						v, rep, checkrange);
////				ofstream tree(fName);
////				iff.printStat(tree);
////				tree.close();
////				if(1 == 1) return 0;
//
//				scores = iff.getScore(dt, 1);
//				sprintf(fName, "%s.OFF.p.min.v%d.r%d.c%d.csv", output_name, v, rep, checkrange);
//				printScoreToFile(scores, metadata, fName);
//
//				scores = iff.getScore(dt, 2);
//				sprintf(fName, "%s.OFF.p.mean.v%d.r%d.c%d.csv", output_name, v, rep, checkrange);
//				printScoreToFile(scores, metadata, fName);
////				if(1 == 1) continue;
////
////				scores = iff.getScore(dt, 3);
////				sprintf(fName, "%s.OFF.p.median.v%d.r%d.c%d.csv", output_name, v, rep, checkrange);
////				printScoreToFile(scores, metadata, fName);
////
////				scores = iff.getScore(dt, 4);
////				sprintf(fName, "%s.OFF.p.max.v%d.r%d.c%d.csv", output_name, v, rep, checkrange);
////				printScoreToFile(scores, metadata, fName);
//
//				scores.clear();
//				scores = iff.AnomalyScore(dt);
//				sprintf(fName, "%s.OFF.v%d.r%d.c%d.csv", output_name, v, rep, checkrange);
//				printScoreToFile(scores, metadata, fName);
//
////				IsolationForest iff2(ntree, normalData, nsample, maxheight, rsample);
////				scores.clear();
////				scores = iff2.AnomalyScore(dt);
////				sprintf(fName, "%s.OFF.v%d.r%d.c%d.tnorm.csv", output_name, v, rep, checkrange);
////				printScoreToFile(scores, metadata, fName);
//
//				for (int wi = 1; wi < 5; ++wi) {
//					windowSize = WINSIZE[wi];
//					if (windowSize > dt->nrow)
//						windowSize = dt->nrow;
//					std::cout << "WS = " << windowSize << std::endl;
//
//					doubleframe *dtOn = copyRows(dt, 0, windowSize - 1);
//
//					// Fixed Tree
//					OnlineIF oif(ntree, dtOn, nsample, maxheight, rsample, windowSize);
//					scores.clear();
//					for (int i = 0; i < windowSize; ++i) {
//						scores.push_back(oif.instanceScore(dtOn->data[i]));
//					}
//
//					deletedoubleframe(dtOn);
////					scoresMin.clear();
////					scoresAvg.clear();
//					for (int i = windowSize; i < dt->nrow; ++i) {
////						if(i % windowSize == 0 || i == dt->nrow-1){
////							dtOn = copyRows(dt, i-windowSize, i - 1);
////							std::vector<double> tmpScores = oif.getScore(dtOn, 1);
////							for (int jj = 0; jj < (int) tmpScores.size(); ++jj) {
////								scoresMin.push_back(tmpScores[jj]);
////							}
////							tmpScores = oif.getScore(dtOn, 2);
////							for (int jj = 0; jj < (int) tmpScores.size(); ++jj) {
////								scoresAvg.push_back(tmpScores[jj]);
////							}
////							deletedoubleframe(dtOn);
////						}
//						scores.push_back(oif.instanceScore(dt->data[i]));
//						oif.update(dt->data[i]);
//					}
//					sprintf(fName, "%s.FTS.v%d.w%d.r%d.c%d.csv", output_name, v,
//							WINSIZE[wi], rep, checkrange);
//					printScoreToFile(scores, metadata, fName);
////					sprintf(fName, "%s.FTS.p.Min.v%d.w%d.r%d.c%d.csv", output_name, v,
////							WINSIZE[wi], rep, checkrange);
////					printScoreToFile(scoresMin, metadata, fName);
////					sprintf(fName, "%s.FTS.p.Mean.v%d.w%d.r%d.c%d.csv", output_name, v,
////							WINSIZE[wi], rep, checkrange);
////					printScoreToFile(scoresAvg, metadata, fName);
//
//					// Adaptive Tree Structure
//					IsolationForest *if0 = NULL, *if1;
//					scores.clear();
////					scoresMin.clear();
////					scoresAvg.clear();
//					for (int st = 0; st <= dt->nrow; st += windowSize) {
//						int end = st + windowSize - 1;
//						if (end >= dt->nrow)
//							end = dt->nrow - 1;
//						doubleframe *dtOn = copyRows(dt, st, end);
//
//						if1 = new IsolationForest(ntree, dtOn, nsample, maxheight, rsample);
//						if (if0 != NULL) {
//							std::vector<double> tmpScores = if0->AnomalyScore(dtOn);
//							for (int jj = 0; jj < (int) tmpScores.size(); ++jj) {
//								scores.push_back(tmpScores[jj]);
//							}
////							tmpScores = if0->getScore(dtOn, 1);
////							for (int jj = 0; jj < (int) tmpScores.size(); ++jj) {
////								scoresMin.push_back(tmpScores[jj]);
////							}
////							tmpScores = if0->getScore(dtOn, 2);
////							for (int jj = 0; jj < (int) tmpScores.size(); ++jj) {
////								scoresAvg.push_back(tmpScores[jj]);
////							}
//							delete if0;
//						} else {
//							std::vector<double> tmpScores = if1->AnomalyScore(dtOn);
//							for (int jj = 0; jj < (int) tmpScores.size(); ++jj) {
//								scores.push_back(tmpScores[jj]);
//							}
////							tmpScores = if1->getScore(dtOn, 1);
////							for (int jj = 0; jj < (int) tmpScores.size(); ++jj) {
////								scoresMin.push_back(tmpScores[jj]);
////							}
////							tmpScores = if1->getScore(dtOn, 2);
////							for (int jj = 0; jj < (int) tmpScores.size(); ++jj) {
////								scoresAvg.push_back(tmpScores[jj]);
////							}
//						}
//						if0 = if1;
//						deletedoubleframe(dtOn);
//					}
//					delete if0;
//					sprintf(fName, "%s.ATS.v%d.w%d.r%d.c%d.csv", output_name, v,
//							WINSIZE[wi], rep, checkrange);
//					printScoreToFile(scores, metadata, fName);
////					sprintf(fName, "%s.ATS.p.Min.v%d.w%d.r%d.c%d.csv", output_name, v,
////							WINSIZE[wi], rep, checkrange);
////					printScoreToFile(scoresMin, metadata, fName);
////					sprintf(fName, "%s.ATS.p.Mean.v%d.w%d.r%d.c%d.csv", output_name, v,
////							WINSIZE[wi], rep, checkrange);
////					printScoreToFile(scoresAvg, metadata, fName);
//				}
//			}
//		}
//	}
////	deletedoubleframe(normalData);
	return 0;
}
