#include "../Common/BigWar.h"
#include "../Common/DS_timer.h"
#include "Grader.h"
//#include "KernelCall.h"
#include "Team.h"
#include <omp.h>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;


int main(int argc, char** argv) {
	if (argc < 4) {
		printf("Usage: exe inputA inputB outputFile GT_file(optional for grading) TeamID(optional for grading)\n");
		exit(1);
	}

	DS_timer timer(1, 1);
	timer.initTimers();
	Pair result[100];

	timer.onTimer(0);
	// **************************************//
	// Write your code here
	// CAUTION: DO NOT MODITY OTHER PART OF THE main() FUNCTION

	FILE* t1fp = NULL;
	FILE* t2fp = NULL;
	UINT t1NumArmies = 0;
	UINT t2NumArmies = 0;
	fopen_s(&t1fp, argv[1], "rb");
	fread_s(&t1NumArmies, sizeof(UINT), sizeof(UINT), 1, t1fp);
	float* t1 = (float*)malloc(sizeof(float) * 3 * t1NumArmies);
	for (int i = 0; i < 3 * t1NumArmies; i++) {
		if (fread_s(&t1[i], sizeof(float), sizeof(float), 1, t1fp) == 0) {
			break;
		}
	}
	fopen_s(&t2fp, argv[2], "rb");
	fread_s(&t2NumArmies, sizeof(UINT), sizeof(UINT), 1, t2fp);
	float* t2 = (float*)malloc(sizeof(float) * 3 * t2NumArmies);
	for (int i = 0; i < 3 * t2NumArmies; i++) {
		if (fread_s(&t2[i], sizeof(float), sizeof(float), 1, t2fp) == 0) {
			break;
		}
	}
	fclose(t1fp);
	fclose(t2fp);
	printf("%d \n", t1NumArmies);
	printf("%d \n", t2NumArmies);
	int memSizeA = (sizeof(float)) * 3 * t1NumArmies;
	int memSizeM = (sizeof(float)) * 3 * t2NumArmies;

	float t1_min[3] = { t1[0] ,t1[1] ,t1[2] };
	float t1_max[3] = { t1[0] ,t1[1] ,t1[2] };
	for (int i = 1; i < t1NumArmies; i++) {
		if (t1[i * 3] < t1_min[0]) t1_min[0] = t1[i * 3];
		if (t1[i * 3 + 1] < t1_min[1]) t1_min[1] = t1[i * 3 + 1];
		if (t1[i * 3 + 2] < t1_min[2]) t1_min[2] = t1[i * 3 + 2];
		if (t1_max[0] < t1[i * 3]) t1_max[0] = t1[i * 3];
		if (t1_max[1] < t1[i * 3 + 1]) t1_max[1] = t1[i * 3 + 1];
		if (t1_max[2] < t1[i * 3 + 2]) t1_max[2] = t1[i * 3 + 2];
	}
	for (int i = 1; i < t2NumArmies; i++) {
		if (t2[i * 3] < t1_min[0]) t1_min[0] = t2[i * 3];
		if (t2[i * 3 + 1] < t1_min[1]) t1_min[1] = t2[i * 3 + 1];
		if (t2[i * 3 + 2] < t1_min[2]) t1_min[2] = t2[i * 3 + 2];
		if (t1_max[0] < t2[i * 3]) t1_max[0] = t2[i * 3];
		if (t1_max[1] < t2[i * 3 + 1]) t1_max[1] = t2[i * 3 + 1];
		if (t1_max[2] < t2[i * 3 + 2]) t1_max[2] = t2[i * 3 + 2];
	}
	
	for (int i = 0; i < 3; i++) {
		t1_min[i] -= 0.001;
		t1_max[i] += 0.001;
	}
	float mid[3] = { (t1_max[0] - t1_min[0]) / 2.0,(t1_max[1] - t1_min[1]) / 2.0 ,(t1_max[2] - t1_min[2]) / 2.0 };

	int counts[8] = { 0, };
	int counts2[8] = { 0, };
	vector < vector <int> > v(8);
	vector < vector <int> > v2(8);
	
#pragma omp parallel num_threads(8)
	{
#pragma omp for
		for (int k = 0; k < 8; k++) {
			for (int i = 0; i < t1NumArmies; i++) {
				switch (k) {
				case 0:
					if ((t1[i * 3] > t1_min[0] && t1[i * 3 + 1] > t1_min[1] && t1[i * 3 + 2] > t1_min[2]) && (t1[i * 3] <= mid[0] && t1[i * 3 + 1] <= mid[1] && t1[i * 3 + 2] <= mid[2])) {
						counts[k]++;
						v[k].push_back(i);
					}
					break;
				case 1:
					if ((t1[i * 3] > mid[0] && t1[i * 3 + 1] > t1_min[1] && t1[i * 3 + 2] > t1_min[2])) {
						if (t1[i * 3] <= t1_max[0] && t1[i * 3 + 1] <= mid[1] && t1[i * 3 + 2] <= mid[2]) {
							counts[k]++;
							v[k].push_back(i);
						}
					}
					break;

				case 2:
					if ((t1[i * 3] > t1_min[0] && t1[i * 3 + 1] > mid[1] && t1[i * 3 + 2] > t1_min[2])) {
						if ((t1[i * 3] <= mid[0] && t1[i * 3 + 1] <= t1_max[1] && t1[i * 3 + 2] <= mid[2])) {
							counts[k]++;
							v[k].push_back(i);
						}
					}
					break;
				case 3:
					if ((t1[i * 3] > mid[0] && t1[i * 3 + 1] > mid[1] && t1[i * 3 + 2] > t1_min[2])) {
						if (t1[i * 3] <= t1_max[0] && t1[i * 3 + 1] <= t1_max[1] && t1[i * 3 + 2] <= mid[2]) {
							counts[k]++;
							v[k].push_back(i);
						}
					}
					break;
				case 4:
					if ((t1[i * 3] > t1_min[0] && t1[i * 3 + 1] > t1_min[1] && t1[i * 3 + 2] > mid[2])) {
						if (t1[i * 3] <= mid[0] && t1[i * 3 + 1] <= mid[1] && t1[i * 3 + 2] <= t1_max[2]) {
							counts[k]++;
							v[k].push_back(i);
						}
					}
				case 5:
					if ((t1[i * 3] > mid[0] && t1[i * 3 + 1] > t1_min[1] && t1[i * 3 + 2] > mid[2])) {
						if (t1[i * 3] <= t1_max[0] && t1[i * 3 + 1] <= mid[1] && t1[i * 3 + 2] <= t1_max[2]) {
							counts[k]++;
							v[k].push_back(i);
						}
					}
					break;
				case 6:
					if ((t1[i * 3] > t1_min[0] && t1[i * 3 + 1] > mid[1] && t1[i * 3 + 2] > mid[2])) {
						if (t1[i * 3] <= mid[0] && t1[i * 3 + 1] <= t1_max[1] && t1[i * 3 + 2] <= t1_max[2]) {
							counts[k]++;
							v[k].push_back(i);
						}
					}

					break;
				case 7:
					if (t1[i * 3] > mid[0] && t1[i * 3 + 1] > mid[1] && t1[i * 3 + 2] > mid[2]) {
						if (t1[i * 3] <= t1_max[0] && t1[i * 3 + 1] <= t1_max[1] && t1[i * 3 + 2] <= t1_max[2]) {
							counts[k]++;
							v[k].push_back(i);
						}
					}
					break;
				}
			}
		}
#pragma omp for
		for (int k = 0; k < 8; k++) {
			for (int i = 0; i < t2NumArmies; i++) {
				switch (k) {
				case 0:
					if ((t2[i * 3] > t1_min[0] && t2[i * 3 + 1] > t1_min[1] && t2[i * 3 + 2] > t1_min[2]) && (t2[i * 3] <= mid[0] && t2[i * 3 + 1] <= mid[1] && t2[i * 3 + 2] <= mid[2])) {
						counts2[k]++;
						v2[k].push_back(i);
					}
					break;
				case 1:
					if ((t2[i * 3] > mid[0] && t2[i * 3 + 1] > t1_min[1] && t2[i * 3 + 2] > t1_min[2])) {
						if (t2[i * 3] <= t1_max[0] && t2[i * 3 + 1] <= mid[1] && t2[i * 3 + 2] <= mid[2]) {
							counts2[k]++;
							v2[k].push_back(i);
						}
					}
					break;

				case 2:
					if ((t2[i * 3] > t1_min[0] && t2[i * 3 + 1] > mid[1] && t2[i * 3 + 2] > t1_min[2])) {
						if ((t2[i * 3] <= mid[0] && t2[i * 3 + 1] <= t1_max[1] && t2[i * 3 + 2] <= mid[2])) {
							counts2[k]++;
							v2[k].push_back(i);
						}
					}
					break;
				case 3:
					if ((t2[i * 3] > mid[0] && t2[i * 3 + 1] > mid[1] && t2[i * 3 + 2] > t1_min[2])) {
						if ((t2[i * 3] <= t1_max[0] && t2[i * 3 + 1] <= t1_max[1] && t2[i * 3 + 2] <= mid[2])) {
							counts2[k]++;
							v2[k].push_back(i);
						}
					}
					break;
				case 4:
					if ((t2[i * 3] > t1_min[0] && t2[i * 3 + 1] > t1_min[1] && t2[i * 3 + 2] > mid[2])) {
						if (t2[i * 3] <= mid[0] && t2[i * 3 + 1] <= mid[1] && t2[i * 3 + 2] <= t1_max[2]) {
							counts2[k]++;
							v2[k].push_back(i);
						}
					}
				case 5:
					if ((t2[i * 3] > mid[0] && t2[i * 3 + 1] > t1_min[1] && t2[i * 3 + 2] > mid[2])) {
						if (t2[i * 3] <= t1_max[0] && t2[i * 3 + 1] <= mid[1] && t2[i * 3 + 2] <= t1_max[2]) {
							counts2[k]++;
							v2[k].push_back(i);
						}
					}
					break;
				case 6:
					if ((t2[i * 3] > t1_min[0] && t2[i * 3 + 1] > mid[1] && t2[i * 3 + 2] > mid[2])) {
						if (t2[i * 3] <= mid[0] && t2[i * 3 + 1] <= t1_max[1] && t2[i * 3 + 2] <= t1_max[2]) {
							counts2[k]++;
							v2[k].push_back(i);
						}
					}

					break;
				case 7:
					if (t2[i * 3] > mid[0] && t2[i * 3 + 1] > mid[1] && t2[i * 3 + 2] > mid[2]) {
						if (t2[i * 3] <= t1_max[0] && t2[i * 3 + 1] <= t1_max[1] && t2[i * 3 + 2] <= t1_max[2]) {
							counts2[k]++;
							v2[k].push_back(i);
						}
					}
					break;
				}
			}
		}
	}
	
	int sum = 0;
	for (int i = 0; i < 8; i++) {
		//printf("%d \n", counts[i]);
		sum += counts[i];
	}
	sum = 0;
	for (int i = 0; i < 8; i++) {
		//printf("%d \n", counts2[i]);
		sum += counts2[i];
	}

	bool ok = false;

	for (int i = 0; i < 100; i++) {
		result[i].dist = 1024 * 1024;
	}
	Pair result2[800];
	
#pragma omp parallel num_threads(8)
	{
		float dis = 0;
		Pair curPair;
		bool istwice = false;
		Pair result3[100];
		for (int i = 0; i < 100; i++) {
			result3[i].dist = 1024 * 1024;
		}
		int i = omp_get_thread_num();
		for (int j = 0; j < v[i].size(); j++) {
			for (int k = 0; k < v2[i].size(); k++) {
				
				dis = sqrt((t1[v[i][j] * 3] - t2[v2[i][k] * 3]) * (t1[v[i][j] * 3] - t2[v2[i][k] * 3]) + (t1[v[i][j] * 3 + 1] - t2[v2[i][k] * 3 + 1]) * (t1[v[i][j] * 3 + 1] - t2[v2[i][k] * 3 + 1]) + (t1[v[i][j] * 3 + 2] - t2[v2[i][k] * 3 + 2]) * (t1[v[i][j] * 3 + 2] - t2[v2[i][k] * 3 + 2]));

				if (isnan(dis)) printf("%d %lf \n", k, dis);

				curPair.set(v[i][j], v2[i][k], dis);
				if (result3[99] > curPair) {
					for (int z = 0; z < 100; z++) {
						if (result3[z].A == curPair.A && result3[z].B == curPair.B) {
							istwice = true;
							break;
						}
					}
					if (!istwice) {

						result3[99] = curPair;
						sort(result3, result3 + 100, Pair::compare);
					}
					istwice = false;
				}
			}
		}
#pragma omp barrier
		for (int k = 0; k < 100; k++) {
			result2[k + i * 100].A = result3[k].A;
			result2[k + i * 100].B = result3[k].B;
			result2[k + i * 100].dist = result3[k].dist;
		}
#pragma omp barrier
	}
	
	sort(result2, result2 + 800, Pair::compare);
	int t = 0;
	for (int k = 0; k < 100; k++) {
		for (int z = 0; z < 100; z++) {
			if (result[z].A == result2[t].A && result[z].B == result2[t].B) {
				t++;
				continue;
			}
		}
		result[k].A = result2[t].A;
		result[k].B = result2[t].B;
		result[k].dist = result2[t].dist;
		t++;
	}
	free(t1);
	free(t2);
	//***************************************//
	timer.offTimer(0);
	timer.printTimer(0);

	// Result validation
	if (argc < 5)
		return 0;

	// Grading mode
	if (argc < 6) {
		printf("Not enough argument for grading\n");
		exit(2);
	}

	Grader grader(argv[4]);
	grader.grading(result);

	FILE* fp = NULL;
	fopen_s(&fp, argv[5], "a");
	if (fp == NULL) {
		printf("Fail to open %s\n", argv[5]);
		exit(3);
	}
	fprintf(fp, "%f\t%d\n", timer.getTimer_ms(0), grader.getNumCorrect());
	fclose(fp);

	return 0;
}