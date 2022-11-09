#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>

#include <chrono>

FILE* init_bruh()
{
	// Current time in seconds
	uint64_t sec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
	// Convert time to string
	char timestr[100];
    sprintf(timestr, "%d", sec);

	// Compose filename including path to SD card and timestamp to ensure unique
    //char fname[100] = "/mnt/extsd/ADXL357_DATA_";
	char fname[100] = "DATA_";
    char csv[] = ".csv";
	strcat(fname, timestr);
    strcat(fname, csv);

	
	// Open a file with fname, write only
	FILE *foutput = fopen(fname, "w");

	// Print header to file
	fprintf(foutput, "Temp [deg C],X_accel [g],Y_accel [g],Z_accel [g]\n");

	return foutput;
}

void add_line(FILE *foutput)
{
    double dummy = 0.03952;
	fprintf(foutput, "%f\n", dummy);
}


int main(int argc, char** argv)
{
    FILE *foutput = init_bruh();

    add_line(foutput);

    // Close the save file
	fclose(foutput);

    return(0);
}