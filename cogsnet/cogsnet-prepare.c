// This program prepares the script for computing CogSNets
// for combinations of model parameters and communication data
// provided as parameters for the invocation of it.
//
// It generates the cogsnet-compute.sh Bash shell script.
// arguments
//
// [1] pathScript - path to script
// [2] pathCogSNetModelParameters with CogSNet parameters' combinations
// [3] pathSurveyDates with information about survey times
// [4] pathEventsData with communication events directory
// [5] pathCogSNetData with information where to store CogSNets
// [6] pathShell - path to shell
// [7] debugging - print verbose info (0/1)

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_NODE_ID_LENGTH 100
#define MAX_FILES 1000

int countLines(char fileName[128]) {
	int numberOfLines = 0;
	char buffer[65536];
	char *line;

	// check if the file exists
	if(access(fileName, F_OK) != -1) {
		FILE* filePointer;
		
		filePointer = fopen(fileName, "r");
			
		// check if there is no other problem with the file stream
		if(filePointer != NULL) {
			while ((line = fgets(buffer, sizeof(buffer), filePointer)) != NULL) {
				numberOfLines++;
			}

			fclose(filePointer);

			return(numberOfLines);
		}
	} else {
		return(-1);
	}
}

// this function returns the element in the CSV organized as three-column one (x;y;timestamp)
// it is used to extract elements both from CSV files

char* returnElementFromCSV(char eventLine[65536], int elementNumber, char delimiter[1]) {
	char *ptr;

	ptr = strtok(eventLine, delimiter);

	int thisLineElementNumber = 0;

	while(ptr != NULL) {
	  	if(thisLineElementNumber == elementNumber) {
	  		return(ptr);
	  	}

		thisLineElementNumber++;

		ptr = strtok(NULL, delimiter);
	}
}

int main(int argc, char** argv) {
	// arguments

	// pathScript - path to script
	char pathScript[128];
	strcpy(pathScript, argv[1]);

	// pathCogSNetModelParameters with CogSNet parameters' combinations
	char pathCogSNetModelParameters[128];
	strcpy(pathCogSNetModelParameters, argv[2]);

	// pathEventsData with communication events directory
	// naming structure telcodata-STUDENTID.csv expected
	char pathEventsData[128];
	strcpy(pathEventsData, argv[3]);

	// pathCogSNetData with information where to store CogSNets
	char pathCogSNetData[128];
	strcpy(pathCogSNetData, argv[4]);

	// pathShell - path to shell
	char pathShell[128];
	strcpy(pathShell, argv[5]);

	// debugging?
	bool debugging = (bool)atoi(argv[6]);

	// the delimiter in the input files
	char delimiter[] = ";";

	printf("[START] Reading CogSNet parameters from %s\n", pathCogSNetModelParameters);
	
	int numberOfLinesModelParameters = 0;
	int numberOfLinesSurveyDates = 0;

	// check if the CogSNet model's parameters file has at least one row with data (excluding header)
	numberOfLinesModelParameters = countLines(pathCogSNetModelParameters);

	if(numberOfLinesModelParameters > 1) {
		if(debugging) {
			printf("[DEBUG] Reading CogSNet parameters: %d lines found (including header)\n", numberOfLinesModelParameters);
		}

		// declare the arrays for parameters
		// one for the forgetting type
		char modelForgettingTypes[numberOfLinesModelParameters - 1][16];
		// the second for all other parameters
		float modelParameters[numberOfLinesModelParameters - 1][7];
		
		char buffer[65536];
		char *line;
		char lineCopy[65536];

		FILE *filePointerParams, *filePointerScript;
		filePointerParams = fopen(pathCogSNetModelParameters, "r");
		filePointerScript = fopen(pathScript, "w");
		
		int lineNumber = 0;
	 	
	 	// read the header line
	 	line = fgets(buffer, sizeof(buffer), filePointerParams);

		// read the CogSNet model parameters' combinations
		for(lineNumber = 0; lineNumber < (numberOfLinesModelParameters - 1); lineNumber++) {
	 		// read the line
	 		line = fgets(buffer, sizeof(buffer), filePointerParams);
			
			// extract the forgetting type
			strcpy(lineCopy, line);
			strcpy(modelForgettingTypes[lineNumber],  returnElementFromCSV(lineCopy, 0, delimiter));

			for(int i = 1; i < 7; i++) {
				// extract other model parameters
				strcpy(lineCopy, line);
				modelParameters[lineNumber][i - 1] = atof(returnElementFromCSV(lineCopy, i, delimiter));
				
			}

			// define single row for single execution
			char singleRow[1024];

			// define a filename for events data
			char cogsnetFilenameSumAll[256];
			char cogsnetFilenameSumWindow[256];
			char cogsnetFilenameAvgAll[256];
			char cogsnetFilenameAvgWindow[256];

			// combine the filename of resulting cogsnets
			snprintf(cogsnetFilenameSumAll, sizeof(cogsnetFilenameSumAll), "%s/cogsnet-%s-%d-%d-%f-%f-%f-%d-sum-all.csv", pathCogSNetData, modelForgettingTypes[lineNumber], (int)modelParameters[lineNumber][0], (int)modelParameters[lineNumber][1], modelParameters[lineNumber][2], modelParameters[lineNumber][3], modelParameters[lineNumber][4], (int)modelParameters[lineNumber][5]);
			snprintf(cogsnetFilenameSumWindow, sizeof(cogsnetFilenameSumWindow), "%s/cogsnet-%s-%d-%d-%f-%f-%f-%d-sum-window.csv", pathCogSNetData, modelForgettingTypes[lineNumber], (int)modelParameters[lineNumber][0], (int)modelParameters[lineNumber][1], modelParameters[lineNumber][2], modelParameters[lineNumber][3], modelParameters[lineNumber][4], (int)modelParameters[lineNumber][5]);
			snprintf(cogsnetFilenameAvgAll, sizeof(cogsnetFilenameAvgAll), "%s/cogsnet-%s-%d-%d-%f-%f-%f-%d-avg-all.csv", pathCogSNetData, modelForgettingTypes[lineNumber], (int)modelParameters[lineNumber][0], (int)modelParameters[lineNumber][1], modelParameters[lineNumber][2], modelParameters[lineNumber][3], modelParameters[lineNumber][4], (int)modelParameters[lineNumber][5]);
			snprintf(cogsnetFilenameAvgWindow, sizeof(cogsnetFilenameAvgWindow), "%s/cogsnet-%s-%d-%d-%f-%f-%f-%d-avg-window.csv", pathCogSNetData, modelForgettingTypes[lineNumber], (int)modelParameters[lineNumber][0], (int)modelParameters[lineNumber][1], modelParameters[lineNumber][2], modelParameters[lineNumber][3], modelParameters[lineNumber][4], (int)modelParameters[lineNumber][5]);

			// write the line
			snprintf(singleRow, sizeof singleRow, "while ! cogsnet/cogsnet-compute %s %d %f %f %f %d %s %s %s %s %s %d ; do echo \"Retrying\"; done", modelForgettingTypes[lineNumber], (int)modelParameters[lineNumber][1], modelParameters[lineNumber][2], modelParameters[lineNumber][3], modelParameters[lineNumber][4], (int)modelParameters[lineNumber][5], pathEventsData, cogsnetFilenameSumAll, cogsnetFilenameSumWindow, cogsnetFilenameAvgAll, cogsnetFilenameAvgWindow, debugging);
			fprintf(filePointerScript, "%s\n", singleRow);

		}

		fclose(filePointerParams);
		fclose(filePointerScript);

		if(debugging) {
			printf("[FINISH] Reading CogSNet parameters: %d model's combinations read\n", numberOfLinesModelParameters - 1);
		}

		// set proper rights to pathScript
		chmod(pathScript, S_IRUSR | S_IWUSR | S_IXUSR);
	} else {
		printf("[ERROR] Reading CogSNet parameters: file %s not found or does not contain data\n", pathCogSNetModelParameters);
	}
}
