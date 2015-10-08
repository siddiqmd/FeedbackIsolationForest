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

	int WINSIZE[] = { 128, 256, 512, 1024, 2048 };
	char fName[100];
	vector<double> scores;
	int windowSize;

	doubleframe *normalData = copyNormalInstances(dt, metadata);

	for(int checkrange = 0; checkrange <= 1; ++checkrange){
		Tree::checkRange = (checkrange == 1);
		std::cout << "CheckRange = " << checkrange << std::endl;
		for (int v = 0; v <= 1; ++v) {
			Tree::useVolumeForScore = (v == 1);
			std::cout << "Volume = " << v << std::endl;
			for (int rep = 0; rep < 10; ++rep) {
				std::cout << "Rep = " << rep << std::endl;

				// standard IsolationForest
				IsolationForest iff(ntree, dt, nsample, maxheight, rsample);

//				sprintf(fName, "D:/ADAPT/OIF/Test/test/tree.v%d.r%d.c%d.txt", v, rep, checkrange);
//				ofstream tree(fName);
//				iff.printStat(tree);
//				tree.close();
//				if(1 == 1) return 0;

				scores.clear();
				scores = iff.AnomalyScore(dt);
				sprintf(fName, "%s.OFF.v%d.r%d.c%d.csv", output_name, v, rep, checkrange);
				printScoreToFile(scores, metadata, fName);

				IsolationForest iff2(ntree, normalData, nsample, maxheight, rsample);
				scores.clear();
				scores = iff2.AnomalyScore(dt);
				sprintf(fName, "%s.OFF.v%d.r%d.c%d.tnorm.csv", output_name, v, rep, checkrange);
				printScoreToFile(scores, metadata, fName);
				if(1 == 1) continue;

				for (int wi = 2; wi < 3; ++wi) {
					windowSize = WINSIZE[wi];
					if (windowSize > dt->nrow)
						windowSize = dt->nrow;
					std::cout << "WS = " << windowSize << std::endl;

					doubleframe *dtOn = copyRows(dt, 0, windowSize - 1);

					// Fixed Tree
					OnlineIF oif(ntree, dtOn, nsample, maxheight, rsample, windowSize);
					scores.clear();
					for (int i = 0; i < windowSize; ++i) {
						scores.push_back(oif.instanceScore(dtOn->data[i]));
					}
					deletedoubleframe(dtOn);
					for (int i = windowSize; i < dt->nrow; ++i) {
						scores.push_back(oif.instanceScore(dt->data[i]));
						oif.update(dt->data[i]);
					}
					sprintf(fName, "%s.FTS.v%d.w%d.r%d.c%d.csv", output_name, v,
							WINSIZE[wi], rep, checkrange);
					printScoreToFile(scores, metadata, fName);

					// Adaptive Tree Structure
					IsolationForest *if0 = NULL, *if1;
					scores.clear();
					for (int st = 0; st <= dt->nrow; st += windowSize) {
						int end = st + windowSize - 1;
						if (end >= dt->nrow)
							end = dt->nrow - 1;
						doubleframe *dtOn = copyRows(dt, st, end);

						if1 = new IsolationForest(ntree, dtOn, nsample, maxheight, rsample);
						if (if0 != NULL) {
							std::vector<double> tmpScores = if0->AnomalyScore(dtOn);
							for (int jj = 0; jj < (int) tmpScores.size(); ++jj) {
								scores.push_back(tmpScores[jj]);
							}
							delete if0;
						} else {
							std::vector<double> tmpScores = if1->AnomalyScore(dtOn);
							for (int jj = 0; jj < (int) tmpScores.size(); ++jj) {
								scores.push_back(tmpScores[jj]);
							}
						}
						if0 = if1;
						deletedoubleframe(dtOn);
					}
					delete if0;
					sprintf(fName, "%s.ATS.v%d.w%d.r%d.c%d.csv", output_name, v,
							WINSIZE[wi], rep, checkrange);
					printScoreToFile(scores, metadata, fName);
				}
			}
		}
	}
	deletedoubleframe(normalData);
	return 0;
}
