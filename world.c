#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <png.h>

#define CELL 20

#define WIDTH (CELL * 360)
#define HEIGHT (CELL * 180)

short *e;
unsigned char *e2;

int main(int argc, char **argv) {
	int seq = 0;

	e = malloc(WIDTH * HEIGHT * sizeof(short));
	memset(e, '\0', WIDTH * HEIGHT * sizeof(short));

	e2 = malloc(WIDTH * HEIGHT * sizeof(unsigned char));
	memset(e2, '\0', WIDTH * HEIGHT * sizeof(unsigned char));

	char s[2000];

	while (fgets(s, 2000, stdin)) {
		char *slash = s;
		char *cp;

		for (cp = s; *cp; cp++) {
			if (*cp == '/') {
				slash = cp;
			}

			if (*cp == '\n') {
				*cp = '\0';
				break;
			}
		}

		char latd[10], lond[10];
		memset(latd, '\0', 10);
		memset(lond, '\0', 10);
		int lat, lon;

		if (sscanf(slash + 1, "%1c%d%1c%d", latd, &lat, lond, &lon) == 4) {
			fprintf(stderr, "%d: %s %d %s %d\n", seq++, latd, lat, lond, lon);
		} else {
			fprintf(stderr, "??? %s\n", s);
			continue;
		}

		if (*latd == 'N') {  // because we want 0 at the top
			lat = -lat;
		}
		if (*lond == 'W') {
			lon = -lon;
		}

		int x = (lon + 180) * CELL;
		int y = (lat + 90) * CELL;

		char p[2000];
		sprintf(p, "unzip -p %s", s);
		FILE *f = popen(p, "r");

		int xx, yy;
		for (yy = 0; yy < 1201; yy++) {
			for (xx = 0; xx < 1201; xx++) {
				int c1 = getc(f);
				int c2 = getc(f);

				int xa = x + xx * CELL / 1200;
				int ya = y + yy * CELL / 1200;

				if (xa < WIDTH && ya < HEIGHT && xa >= 0 && ya >= 0) {
					short v = (c1 << 8) | c2;

					if (1) {
						if (v > e[ya * WIDTH + xa]) {
							e[ya * WIDTH + xa] = v;
						}
					}
				}
			}
		}

		pclose(f);
	}

        png_image image;

        memset(&image, 0, sizeof image);
        image.version = PNG_IMAGE_VERSION;
        image.format = PNG_FORMAT_GRAY;
        image.width = WIDTH;
        image.height = HEIGHT;

	short max = 0;
	int i;
	for (i = 0; i < WIDTH * HEIGHT; i++) {
		if (e[i] > max) {
			max = e[i];
		}
	}

	for (i = 0; i < WIDTH * HEIGHT; i++) {
		e2[i] = sqrt(e[i]) * 255 / sqrt(max);
	}

        png_image_write_to_stdio(&image, stdout, 0, e2, WIDTH, NULL);
        png_image_free(&image);
}

