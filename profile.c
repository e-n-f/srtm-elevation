#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>

char *dirs[] = {
	"Africa",
	"Australia",
	"Eurasia",
	"Islands",
	"North_America",
	"South_America",
	NULL
};

struct srtm3 {
	int key;
	short *data;

	struct srtm3 *left;
	struct srtm3 *right;
};

struct srtm3 *root = NULL;
struct srtm3 *find(int ilat, int ilon, struct srtm3 **sr) {
	int key = (ilat + 360) * 360 + ilon;

	while (*sr != NULL) {
		if ((*sr)->key < key) {
			sr = &((*sr)->left);
		} else if ((*sr)->key > key) {
			sr = &((*sr)->right);
		} else {
			return *sr;
		}
	}

	*sr = malloc(sizeof(struct srtm3));
	(*sr)->left = NULL;
	(*sr)->right = NULL;
	(*sr)->key = key;
	(*sr)->data = malloc(1201 * 1201 * sizeof(short));

	int i;
	for (i = 0; i < 1201 * 1201; i++) {
		(*sr)->data[i] = -1;
	}

	char **cp;
	for (cp = dirs; *cp; cp++) {
		char path[2000];
		sprintf(path, "dds.cr.usgs.gov/srtm/version2_1/SRTM3/%s/%c%02d%c%03d.hgt.zip",
			*cp,
			ilat >= 0 ? 'N' : 'S', ilat >= 0 ? ilat : abs(ilat),
			ilon >= 0 ? 'E' : 'W', ilon >= 0 ? ilon : abs(ilon));

		//fprintf(stderr, "try %s\n", path);

		struct stat st;
		if (stat(path, &st) == 0) {
			char stream[2000];
			sprintf(stream, "unzip -p %s", path);

			FILE *fp = popen(stream, "r");
			fprintf(stderr, "loading %s\n", stream);
			if (fp == NULL) {
				perror(stream);
				break;
			}
			int xx, yy;
			for (yy = 0; yy < 1201; yy++) {
				for (xx = 0; xx < 1201; xx++) {
					int c1 = getc(fp);
					int c2 = getc(fp);
					short v = (c1 << 8) | c2;

					(*sr)->data[1201 * yy + xx] = v;
				}
			}
			pclose(fp);
			
			break;
		}
	}

	return *sr;
}

int lookup(double lat, double lon) {
	int ilat = floor(lat);
	int ilon = floor(lon);

	struct srtm3 *srtm = find(ilat, ilon, &root);

	int latpart = (lat - floor(lat)) * 1200;
	int lonpart = (lon - floor(lon)) * 1200;

	//printf("%d,%d\n", latpart, lonpart);

#if 1
	// 1199 because latitude part 0 is the bottom row
	// row 1200 is duplicated from the tile below
	return srtm->data[1201 * (1199 - latpart) + lonpart];
#else
	return srtm->data[1201 * latpart + 1199 - lonpart];
#endif
}

int main(int argc, char **argv) {
	while (1) {
		int c = getchar();
		if (c == EOF) {
			break;
		}

		ungetc(c, stdin);
		if (isdigit(c) || c == '-') {
			double lat, lon;

			if (scanf("%lf,%lf", &lat, &lon) == 2) {
				printf("%f,%f,%d", lat, lon, lookup(lat, lon));
			} else {
				c = getchar();
				putchar(c);
			}
		} else {
			c = getchar();
			putchar(c);
			if (c == ':') {
				while ((c = getchar()) != EOF) {
					putchar(c);
					if (c == ' ' || c == '\n') {
						break;
					}
				}
			}
		}
	}
}
