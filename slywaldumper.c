#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define SECTOR_SIZE 2048

/*
	Sector lookup tables:
		If value is -1 extracting that builds wal is not supported.
*/

const int32_t startSectors[3][3] = {
	// Sly 1
	{
		12345, // PAL
		12345, // NTSC-U
		12345  // NTSC-J
	},
	
	// Sly 2
	{
		3232, // PAL
		-1,   // NTSC-U
		-1    // NTSC-J
	},
	
	// Sly3
	{
		3232, // PAL
		-1,   // NTSC-U
		-1    // NTSC-J
	},
};

const int32_t endSectors[3][3] = {
	// Sly 1
	{
		1427632, // PAL
		1368306, // NTSC-U
		955136   // NTSC-J
	},
	
	// Sly 2
	{
		-1, // PAL
		-1, // NTSC-U
		-1  // NTSC-J
	},
	
	// Sly 3
	{
		2135015, // PAL
		-1,      // NTSC-U
		-1       // NTSC-J
	}
};

int getGameRegion(const char *regionName) {
	if(strcmp(regionName, "pal") == 0) {
		return 0;
	}
	else if(strcmp(regionName, "ntsc-u") == 0) {
		return 1;
	}
	else if(strcmp(regionName, "ntsc-j") == 0) {
		return 2;
	}
	else {
		return 0;
	}
}

void extactWal(const char *isoFilename, const char *walFilename) {
	FILE *slyIso = fopen(isoFilename, "rb");
	FILE *slyWal = fopen(walFilename, "wb");
	
	fseek(slyIso, 0, SEEK_SET);
	
	printf("Copying sectors to %s\n", walFilename);
	
	uint8_t sectorData[SECTOR_SIZE];
	for(int i = 0; i < 4; i++) {
		fread(&sectorData, SECTOR_SIZE, 1, slyIso);
		fwrite(&sectorData, SECTOR_SIZE, 1, slyWal);
	}
	
	printf("Done!\n");
	
	fclose(slyWal);
	fclose(slyIso);
}

int main(int argc, char *argv[]) {
	printf("SlyWalDumper v1.0\n");
	printf("Made by: 545u\n");
	printf("https://github.com/545u\n\n");
	
	if(argc == 5) {
		int gameRegion = getGameRegion(argv[3]);
		int gameNumber = argv[4][0] - '0';
		
		if((strcmp(argv[3], "pal") != 0) && (strcmp(argv[3], "ntsc-u") != 0) && (strcmp(argv[3], "ntsc-j") != 0)) {
			printf("Error: Invalid or unsupported region!\n");
			return EXIT_FAILURE;
		}
		
		uint32_t startSector = startSectors[gameNumber - 1][gameRegion];
		uint32_t endSector = endSectors[gameNumber - 1][gameRegion];
		
		if((gameNumber > 3 || gameNumber < 1) || startSector == -1 || endSector == -1) {
			printf("Error: Invalid or unsupported game!\n");
			return EXIT_FAILURE;
		}
		
		FILE *slyIso = fopen(argv[1], "rb");
		FILE *slyWal = fopen(argv[2], "wb");
		
		fseek(slyIso, startSector * SECTOR_SIZE, SEEK_SET);
		
		printf("Copying sectors to %s\n", argv[2]);
		
		uint8_t sectorData[SECTOR_SIZE];
		for(int i = 0; i < endSector - startSector + 1; i++) {
			fread(&sectorData, SECTOR_SIZE, 1, slyIso);
			fwrite(&sectorData, SECTOR_SIZE, 1, slyWal);
		}
		
		printf("Done!\n");
		
		fclose(slyWal);
		fclose(slyIso);
	}
	else {
		printf("Usage:\n");
		printf("     %s [Sly Cooper PS2 Iso] [Output Wal Filename] [Game Region] [Game Number]\n\n", argv[0]);
		printf("Regions:\n");
		printf("     pal, ntsc-u, ntsc-j\n\n");
		printf("Supported builds:\n");
		printf("     Sly 1 (PAL, NTSC-U, NTSC-J)\n");
		printf("     Sly 3 (PAL)\n\n");
		printf("Example Usage:\n");
		printf("     %s Sly1_PAL.iso SLY1.WAL pal 1\n", argv[0]);
		printf("     %s Sly3_NTSC-U.iso SLY3.WAL ntsc-u 3\n", argv[0]);
	}
	
	return EXIT_SUCCESS;
}