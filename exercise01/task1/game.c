#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ALIVE 0
#define DEAD 1

void runGame(bool** field1, bool** field2, int width, int height, int stepCounter, int steps);
void saveCurrentFrame(bool** field, int width, int height, int step);
int getSurroundingAlive(bool** field, int width, int height, int x, int y);

bool allocatePlayingField(bool*** field, int width, int height);
void initPlayingField(bool*** playingField, int width, int height, double density);
void generatePBM(bool** playingField, int width, int height, int iteration);

void freePlayingField(bool** playingField, int width);

void printUsage(const char* programName) {
	printf("usage: %s <width> <height> <density> <steps>\n", programName);
}

int main(int argc, char* argv[]) {
	if (argc != 5) {
		printUsage(argv[0]);
		return EXIT_FAILURE;
	}

	const int width = atoi(argv[1]);
	const int height = atoi(argv[2]);
	const double density = atof(argv[3]);
	const int steps = atoi(argv[4]);

	printf("width:   %4d\n", width);
	printf("height:  %4d\n", height);
	printf("density: %4.0f%%\n", density * 100);
	printf("steps:   %4d\n", steps);

	// Seeding the random number generator so we get a different starting field
	// every time.
	srand(time(NULL));

	bool** field1 = NULL;
	bool** field2 = NULL;
	bool alloc1Fail = allocatePlayingField(&field1, width, height);
	bool alloc2Fail = allocatePlayingField(&field2, width, height);

	if (alloc1Fail || alloc2Fail) {
		freePlayingField(field1, width);
		freePlayingField(field2, width);

		exit(1);
	}

	initPlayingField(&field1, width, height, density);
	saveCurrentFrame(field1, width, height, 0);

	runGame(field1, field2, width, height, 1, steps);

	freePlayingField(field1, width);
	freePlayingField(field2, width);

	return EXIT_SUCCESS;
}

void runGame(bool** field1, bool** field2, int width, int height, int stepCounter, int steps) {
	int surroundingSum;
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			surroundingSum = getSurroundingAlive(field1, width, height, x, y);

			bool isAlive = field1[x][y] == ALIVE;
			if (isAlive && (surroundingSum < 2 || surroundingSum > 3)) {
				field2[x][y] = DEAD;
			} else if (!isAlive && surroundingSum == 3) {
				field2[x][y] = ALIVE;
			} else {
				field2[x][y] = field1[x][y];
			}
		}
	}

	saveCurrentFrame(field2, width, height, stepCounter);
	if (stepCounter < steps) runGame(field2, field1, width, height, ++stepCounter, steps);
}

int getSurroundingAlive(bool** field, int width, int height, int x, int y) {
	int sum = 0;

	int xLowerBound = x == 0 ? 0 : -1;
	int xUpperBound = x == (width - 1) ? 0 : 1;

	int yLowerBound = y == 0 ? 0 : -1;
	int yUpperBound = y == (height - 1) ? 0 : 1;

	for (int xDelta = xLowerBound; xDelta <= xUpperBound; xDelta++) {
		for (int yDelta = yLowerBound; yDelta <= yUpperBound; yDelta++) {
			if (xDelta == 0 && yDelta == 0) continue;
			sum += field[x + xDelta][y + yDelta] == ALIVE ? 1 : 0;
		}
	}

	return sum;
}

void saveCurrentFrame(bool** field, int width, int height, int step) {
	char name[14];
	snprintf(name, 14, "gol_%05d.pbm", step);

	FILE* outFile = fopen(name, "w");
	if (outFile == NULL) {
		return;
	}

	fprintf(outFile, "P1\n");
	fprintf(outFile, "%d %d\n", width, height);

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			fprintf(outFile, "%d", field[i][j]);
		}
		fprintf(outFile, "\n");
	}

	fclose(outFile);
}

bool allocatePlayingField(bool*** field, int width, int height) {
	*field = malloc(width * sizeof(*field));
	if (*field == NULL) {
		fprintf(stderr, "Failed to allocate playing field");
		return 1;
	}

	bool arrFailure = false;
	for (int i = 0; i < width; i++) {
		(*field)[i] = calloc(height, sizeof(**field));

		if ((*field)[i] == NULL) {
			arrFailure = true;
			break;
		}
	}

	if (arrFailure) {
		fprintf(stderr, "Failed to allocate playing field");

		for (int i = 0; i < width; i++) {
			free((*field)[i]);
		}

		free(*field);
		return 1;
	}

	return 0;
}

void initPlayingField(bool*** field1, int width, int height, double density) {
	double randDoub;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			randDoub = (double)rand() / RAND_MAX;
			(*field1)[i][j] = randDoub < density ? ALIVE : DEAD;
		}
	}
}

void freePlayingField(bool** field, int width) {
	if (field == NULL) {
		return;
	}

	for (int i = 0; i < width; i++) {
		free(field[i]);
	}

	free(field);
}
