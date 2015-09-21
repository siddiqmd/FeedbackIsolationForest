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

int sidx = 0;
struct sItem{
	double *p;
	ntstring *q;
};
int cmp(sItem *a, sItem *b){
	if(a->p[sidx] > b->p[sidx]) return 1;
	if(a->p[sidx] < b->p[sidx]) return -1;
	return 0;
}

void printData(int fid, doubleframe* dt, ntstringframe* metadata){
	char fname[100];
	sprintf(fname, "data%d.csv", fid+1);
	std::ofstream out(fname);
	for(int i = 0; i < dt->nrow; ++i){
		out << metadata->data[i][0];
		for(int j = 0; j < dt->ncol; ++j){
			out << "," << dt->data[i][j];
		}
		out << std::endl;
	}
	out.close();
}
void sortByFeature(int fid, doubleframe* dt, ntstringframe* metadata){
	sidx = fid;
	sItem *tp = new sItem[dt->nrow];
	for(int i = 0; i < dt->nrow; ++i){
		tp[i].p = dt->data[i];
		tp[i].q = metadata->data[i];
	}
	qsort(tp, dt->nrow, sizeof(tp[0]), (int (*)(const void *, const void *))cmp);
	for(int i = 0; i < dt->nrow; ++i){
		dt->data[i] = tp[i].p;
		metadata->data[i] = tp[i].q;
	}
	delete []tp;
}

// randomly shuffle dt along with metadata
void randomShuffle(doubleframe* dt, ntstringframe* metadata){
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

void printScoreToFile(vector<double> &scores, const ntstringframe* metadata, char fName[]){
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
	bool stopheight = maxheight != 0;
	int useColumns = pargs->columns;
//	std::cout << useColumns << std::endl;
//	int windowSize = pargs->window_size;

	ntstringframe* csv = read_csv(input_name, header, false, false);
	ntstringframe* metadata = split_frame(ntstring, csv, metacol, true);
	doubleframe* dt = conv_frame(double, ntstring, csv); //read data to the global variable
	nsample = nsample == 0 ? dt->nrow : nsample;

	std::cout << "Original Data Dimension: " << dt->nrow << "," << dt->ncol << std::endl;

//	if(1 == 1){
//		printData(-1, dt, metadata);
//		for(int ii = 0; ii < dt->ncol; ++ii){
//			sortByFeature(ii, dt, metadata);
//			std::cout << ii << std::endl;
//			printData(ii, dt, metadata);
//		}
//		return 0;
//	}
	if(useColumns > 0 && dt->ncol > useColumns){
		doubleframe* temp = copyCols(dt, 0, useColumns-1);
		dt = temp;
		std::cout << "Using columns: " << dt->ncol << std::endl;
	}

	int WINSIZE[] = { 128, 256, 512, 1024, 2048 };
	char fName[100], statFileName[100];
	vector<double> scores, scores2;
	ofstream outStat;
	int windowSize;
	// print statistics
	sprintf(statFileName, "%s.stat.csv", output_name);
	outStat.open(statFileName);
	outStat << "type,windowsize,repeat,total,numDrift,numDriftHandled" << std::endl;


	for (int rep = 0; rep <= dt->ncol; ++rep) {
		std::cout << "Rep = " << rep << std::endl;

		// standard IsolationForest
		IsolationForest iff(ntree, dt, nsample, maxheight, stopheight, rsample);
		Tree::total = 0;
		Tree::numDrifts = 0;
		Tree::numDriftsHandled = 0;
		// print statistics
//		sprintf(statFileName, "%s.stat.offline.r%d.csv", output_name, rep);
//		outStat.open(statFileName);
//		outStat << "depth,nodesize" << std::endl;
//		iff.printStat(outStat);
//		outStat.close();

		// print score along with ground truth
		scores.clear();
		scores = iff.AnomalyScore(dt);
		sprintf(fName, "%s.score.offline.r%d.csv", output_name, rep);
		printScoreToFile(scores, metadata, fName);
		outStat << "offline,NA," << rep << "," << Tree::total << "," << Tree::numDrifts << ","
				<< Tree::numDriftsHandled << std::endl;

		for (int wi = 0; wi < 5; ++wi) {
			windowSize = WINSIZE[wi];
			if(windowSize > dt->nrow)
				windowSize = dt->nrow;
			std::cout << "WS = " << windowSize << std::endl;

			doubleframe *dtOn = copyRows(dt, 0, windowSize - 1);

			// Online Isolation Forest
			OnlineIF oif(ntree, dtOn, nsample, maxheight, stopheight, rsample,
					windowSize);
			Tree::total = 0;
			Tree::numDrifts = 0;
			Tree::numDriftsHandled = 0;

			scores.clear();
			for (int i = 0; i < windowSize; ++i) {
				scores.push_back(oif.instanceScore(dtOn->data[i]));
			}
			deletedoubleframe(dtOn);

			// print statistics
//			sprintf(statFileName, "%s.stat.online.w%d.r%d.csv", output_name,
//					windowSize, rep);
//			outStat.open(statFileName);
//			outStat << "depth,nodesize" << std::endl;
			for (int i = windowSize; i < dt->nrow; ++i) {
//				if (i % windowSize == 0) {
//					oif.printStat(outStat);
//				}
				scores.push_back(oif.instanceScore(dt->data[i]));
				oif.update(dt->data[i]);
			}
//			oif.printStat(outStat);
//			outStat.close();

			// print score
			sprintf(fName, "%s.score.online.w%d.r%d.csv", output_name,
					WINSIZE[wi], rep);
			printScoreToFile(scores, metadata, fName);
			outStat << "online," << windowSize << "," << rep << "," << Tree::total << ","
					<< Tree::numDrifts << "," << Tree::numDriftsHandled << std::endl;

			// offline isolation forest with window
			IsolationForest *if0 = NULL, *if1;
//			sprintf(statFileName, "%s.stat.offline.w%d.r%d.csv", output_name, windowSize, rep);
//			outStat.open(statFileName);
//			outStat << "depth,nodesize" << std::endl;
			long temptotal = 0, tempDrifts = 0, tempDriftsHandled = 0;
			scores.clear();
			scores2.clear();
			for(int st = 0; st <= dt->nrow; st += windowSize){
//				std::cout << st << std::endl;
				int end = st + windowSize - 1;
				if(end >= dt->nrow)
					end = dt->nrow-1;
				doubleframe *dtOn = copyRows(dt, st, end);

				Tree::total = 0;
				Tree::numDrifts = 0;
				Tree::numDriftsHandled = 0;

				if1 = new IsolationForest(ntree, dtOn, nsample, maxheight, stopheight, rsample);

				// print statistics
//				if1->printStat(outStat);

				std::vector<double> tmpScores = if1->AnomalyScore(dtOn);
				temptotal += Tree::total;
				tempDrifts += Tree::numDrifts;
				tempDriftsHandled += Tree::numDriftsHandled;
				for(int jj = 0; jj < (int)tmpScores.size(); ++jj){
					scores.push_back(tmpScores[jj]);
				}
				if(if0 != NULL){
					std::vector<double> tmpScores = if0->AnomalyScore(dtOn);
					for(int jj = 0; jj < (int)tmpScores.size(); ++jj){
						scores2.push_back(tmpScores[jj]);
					}
					delete if0;
				}else{
					for(int jj = 0; jj < (int)tmpScores.size(); ++jj){
						scores2.push_back(tmpScores[jj]);
					}
				}
				if0 = if1;
				deletedoubleframe(dtOn);
			}
			delete if0;
			outStat << "offline.window," << windowSize << "," << rep << "," << temptotal << ","
					<< tempDrifts << "," << tempDriftsHandled << std::endl;
//			outStat.close();
			// print score along with ground truth
			sprintf(fName, "%s.score.offline.w%d.r%d.csv", output_name, WINSIZE[wi], rep);
			printScoreToFile(scores, metadata, fName);
			sprintf(fName, "%s.score.online.vartree.w%d.r%d.csv", output_name, WINSIZE[wi], rep);
			printScoreToFile(scores2, metadata, fName);
		}
//		randomShuffle(dt, metadata);
		if(rep < dt->ncol)
			sortByFeature(rep, dt, metadata);
	}
	return 0;
}
