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
	int useColumns = 0;
//	int trainsampIdx = pargs->columns;//-c for train sample size option
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

	if (useColumns > 0 && dt->ncol > useColumns) {
		doubleframe* temp = copyCols(dt, 0, useColumns - 1);
		dt = temp;
		std::cout << "Using columns: " << dt->ncol << std::endl;
	}

	char fname[100], fname_marg[100], fname_drop[100], temp[100];
	ofstream fout;

	IsolationForest iff(ntree, dt, nsample, maxheight, rsample);
//	std::ofstream out("out/tree.txt");
//	iff.printStat(out);

	std::vector<double> scores = iff.AnomalyScore(dt);
	sprintf(fname, "%s.csv", output_name);
	printScoreToFile(scores, metadata, fname);

	strcpy(fname_marg, output_name);

	bool *marginalize = new bool[dt->ncol];
	for(int i = 0; i < dt->ncol; i++)
		marginalize[i] = false;

	for(int rep = 0; rep < 5; rep++){
		int *freq = new int[dt->ncol];
		for(int i = 0; i < dt->ncol; i++)
			freq[i] = 0;

		for(int top = 0; top < 100; top++){
			double max = 0;
			int midx = 0;
			for(int i = 0; i < (int)scores.size(); i++){
				if(scores[i] > max && strcmp(metadata->data[i][0], "nominal") == 0){
					max = scores[i];
					midx = i;
				}
			}
//			std::cout << midx << " ";
			scores[midx] = 0;

			std::vector<int> exp_seq_marg = iff.getSeqMarExplanation(dt->data[midx], dt->ncol, 1);
			freq[exp_seq_marg[0]]++;
		}

		int marg_feat = 0;
		int max = 0;
		for(int i = 0; i < dt->ncol; i++){
			if(freq[i] > max){
				max = freq[i];
				marg_feat = i;
			}
		}

		marginalize[marg_feat] = true;
		scores = iff.AnomalyScore(dt, marginalize);
		sprintf(temp, "_%d", marg_feat);
		strcat(fname_marg, temp);
		sprintf(fname, "%s_seq_marg.csv", fname_marg);
		printScoreToFile(scores, metadata, fname);

//		cout << "\n";
	}

	scores = iff.AnomalyScore(dt);
	strcpy(fname_drop, output_name);
	for(int i = 0; i < dt->ncol; i++)
		marginalize[i] = false;

	for(int rep = 0; rep < 5; rep++){
		int *freq = new int[dt->ncol];
		for(int i = 0; i < dt->ncol; i++)
			freq[i] = 0;

		for(int top = 0; top < 100; top++){
			double max = 0;
			int midx = 0;
			for(int i = 0; i < (int)scores.size(); i++){
				if(scores[i] > max && strcmp(metadata->data[i][0], "nominal") == 0){
					max = scores[i];
					midx = i;
				}
			}
//			std::cout << midx << " ";
			scores[midx] = 0;

			std::vector<int> exp_seq_drop = iff.getSeqDropExplanation(dt->data[midx], dt->ncol, 1);
			freq[exp_seq_drop[0]]++;
		}

		int marg_feat = 0;
		int max = 0;
		for(int i = 0; i < dt->ncol; i++){
			if(freq[i] > max){
				max = freq[i];
				marg_feat = i;
			}
		}

		marginalize[marg_feat] = true;
		scores = iff.AnomalyScore(dt, marginalize);
		sprintf(temp, "_%d", marg_feat);
		strcat(fname_drop, temp);
		sprintf(fname, "%s_seq_drop.csv", fname_drop);
		printScoreToFile(scores, metadata, fname);

//		cout << "\n";
	}

//	// sequential marginal
//	sprintf(fname, "%s.SeqMarg.csv", output_name);
//	fout.open(fname);
//	fout << "groundtruth,anomalyscore";
//	for(int i = 1; i <= dt->ncol; i++)
//		fout << ",R" << i;
//	fout << "\n";
//	for(int i = 0; i < dt->nrow; i++){
//		std::vector<int> explanation = iff.getSeqMarExplanation(dt->data[i], dt->ncol);
//		fout << metadata->data[i][0] << "," << scores[i];
//		for(int j = 0; j < (int)explanation.size(); j++)
//			fout << "," << explanation[j]+1;
//		fout << "\n";
//	}
//	fout.close();
//
//	// sequential dropout
//	sprintf(fname, "%s.SeqDrop.csv", output_name);
//	fout.open(fname);
//	fout << "groundtruth,anomalyscore";
//	for(int i = 1; i <= dt->ncol; i++)
//		fout << ",R" << i;
//	fout << "\n";
//	for(int i = 0; i < dt->nrow; i++){
//		std::vector<int> explanation = iff.getSeqDropExplanation(dt->data[i], dt->ncol);
//		fout << metadata->data[i][0] << "," << scores[i];
//		for(int j = 0; j < (int)explanation.size(); j++)
//			fout << "," << explanation[j]+1;
//		fout << "\n";
//	}
//	fout.close();

	return 0;
}
