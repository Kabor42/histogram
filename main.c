#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _s_hist_elem hist_elem;
typedef struct _s_kep t_kep;
typedef struct _s_rgb t_rgb;
typedef struct _s_ycrcb t_ycrcb;
typedef struct _s_hist histogram;
struct _s_kep {
  unsigned char *tomb;
  size_t szelesseg;
  size_t magassag;
  size_t elemszam;
};
struct _s_rgb {
  unsigned char *r;
  unsigned char *g;
  unsigned char *b;
  size_t szelesseg;
  size_t magassag;
  size_t elemszam;
};
struct _s_ycrcb {
  unsigned char *y;
  unsigned char *cr;
  unsigned char *cb;
  size_t szelesseg;
  size_t magassag;
  size_t elemszam;
};
struct _s_hist_elem {
  unsigned char intervallum;
  size_t elemszam;
};
struct _s_hist {
  hist_elem *tomb;
  size_t elemszam;
};

void betoltes(t_kep *, char *);
void betolt_rgb(t_rgb *, char *);
void hisztogram_keszit(t_kep *, histogram *, unsigned int);
void histogram_kiir(histogram *, char *);
void kumul_eloszlas(histogram *, histogram *);
void uj_ertek(histogram *, histogram *, size_t);
void normalize(t_kep *, t_kep *, histogram *);
void normalize_RGB(t_rgb *);
void kiiras(t_kep *, char *);
void kiir_rgb(t_rgb *, char *);

int main(int argc, char *argv[]) {
  if (argc < 3)
    return -1;
  char fajlnev[strlen(argv[1] + 1)];
  strcpy(fajlnev, argv[1]);
  printf("%s\n", fajlnev);

  t_kep kep, uj_kep;
  histogram hist_16, hist, uj_hist;
  histogram eloszl, uj_eloszl;
  histogram uj_ertekek;
  betoltes(&kep, fajlnev);

  hisztogram_keszit(&kep, &hist_16, 16);
  histogram_kiir(&hist_16, "hist.dat");

  hisztogram_keszit(&kep, &hist, 256);
  kumul_eloszlas(&hist, &eloszl);
  histogram_kiir(&hist, "photo1c1280x720_hist.dat");
  histogram_kiir(&eloszl, "kum.dat");

  uj_ertek(&eloszl, &uj_ertekek, kep.elemszam);
  histogram_kiir(&uj_ertekek, "u.dat");

  normalize(&kep, &uj_kep, &uj_ertekek);

  kiiras(&uj_kep, "fájlnév");

  hisztogram_keszit(&uj_kep, &uj_hist, 256);
  kumul_eloszlas(&uj_hist, &uj_eloszl);
  histogram_kiir(&uj_hist, "fajlnev_hist.dat");
  histogram_kiir(&uj_eloszl, "fajlnev_kum.dat");

  t_rgb rgb;
  betolt_rgb(&rgb, argv[2]);
  normalize_RGB(&rgb);
  kiir_rgb(&rgb, "screenshot480x270_norm.dat");
  

  free(hist.tomb);
  free(hist_16.tomb);
  free(uj_hist.tomb);
  free(eloszl.tomb);
  free(uj_eloszl.tomb);
  free(kep.tomb);
  free(uj_kep.tomb);
  free(uj_ertekek.tomb);
  free(rgb.r);free(rgb.g);free(rgb.b);
  return 0;
}

void betoltes(t_kep *kep, char *fajlnev) {
  FILE *fp = fopen(fajlnev, "r");
  if (NULL == fp)
    return;
  int szelesseg = 0, magassag = 0;
  size_t idx = 0;
  for (idx = 0; idx < strlen(fajlnev) - 1; idx++) {
    if (fajlnev[idx] <= '9' && fajlnev[idx] >= '0' && fajlnev[idx + 1] == 'x') {
      idx++;
      break;
    }
  }
  int i = idx - 1;
  int j = idx + 1;
  while (fajlnev[i] <= '9' && fajlnev[i] >= '0') {
    i--;
  }
  while (fajlnev[j] <= '9' && fajlnev[j] >= '0') {
    j++;
  }
  for (size_t i_m = i + 1; i_m < idx; i_m++) {
    magassag *= 10;
    magassag += fajlnev[i_m] - 48;
  }
  for (size_t i_s = idx + 1; i_s < j; i_s++) {
    szelesseg *= 10;
    szelesseg += fajlnev[i_s] - 48;
  }

  printf("[ DEBUG ][ KEP ][ SZELESSEG ]:%d\n", szelesseg);
  printf("[ DEBUG ][ KEP ][ MAGASSAG ]:%d\n", magassag);
  unsigned char *uj_tomb =
      (unsigned char *)malloc(sizeof(unsigned char) * (szelesseg * magassag));
  kep->tomb = uj_tomb;
  kep->magassag = magassag;
  kep->szelesseg = szelesseg;
  kep->elemszam = magassag * szelesseg;
  int item;
  for (size_t i = 0; i < (szelesseg * magassag); i++) {
    fscanf(fp, " %d", &item);
    uj_tomb[i] = (unsigned char)item;
  }
  fclose(fp);
}

void hisztogram_keszit(t_kep *kep, histogram *hiszt, unsigned int osztas) {
  unsigned char dv = (unsigned char)floor(256 / osztas);
  hiszt->tomb = (hist_elem *)malloc(sizeof(hist_elem) * osztas);
  hiszt->elemszam = osztas;
  for (size_t i = 0; i < hiszt->elemszam; i++) {
    hiszt->tomb[i].intervallum = (i * dv) + (unsigned char)floor(dv / 2);
    hiszt->tomb[i].elemszam = 0;
  }
  for (size_t i = 0; i < kep->elemszam; i++) {
    size_t idx = floor(kep->tomb[i] / dv);
    hiszt->tomb[idx].elemszam++;
  }
}
void histogram_kiir(histogram *hiszt, char *fajlnev) {
  FILE *fp = fopen(fajlnev, "w");
  if (NULL == fp)
    return;
  for (size_t i = 0; i < hiszt->elemszam; i++) {
    fprintf(fp, "%d\t%ld\n", hiszt->tomb[i].intervallum,
            hiszt->tomb[i].elemszam);
  }
  fclose(fp);
}
void kumul_eloszlas(histogram *hist, histogram *eloszl) {
  eloszl->tomb = (hist_elem *)malloc(sizeof(hist_elem) * hist->elemszam);
  eloszl->elemszam = hist->elemszam;
  for (size_t i = 0; i < eloszl->elemszam; i++)
    eloszl->tomb[i].elemszam = 0;
  for (size_t i = 0; i < hist->elemszam; i++) {
    eloszl->tomb[i].intervallum = hist->tomb[i].intervallum;
    for (size_t j = 0; j <= i; j++) {
      eloszl->tomb[i].elemszam += hist->tomb[j].elemszam;
    }
  }
}
void uj_ertek(histogram *eloszl, histogram *uj, size_t total) {
  uj->tomb = (hist_elem *)malloc(sizeof(hist_elem) * eloszl->elemszam);
  uj->elemszam = eloszl->elemszam;
  for (size_t i = 0; i < uj->elemszam; i++) {
    uj->tomb[i].elemszam =
        (size_t)floor(255 * ((double)eloszl->tomb[i].elemszam / total));
    uj->tomb[i].intervallum = eloszl->tomb[i].intervallum;
  }
}
void normalize(t_kep *eredeti, t_kep *uj, histogram *uj_ertekek) {
  unsigned char *uj_tomb =
      (unsigned char *)malloc(sizeof(unsigned char) * eredeti->elemszam);
  uj->tomb = uj_tomb;
  uj->szelesseg = eredeti->szelesseg;
  uj->magassag = eredeti->magassag;
  uj->elemszam = eredeti->elemszam;
  for (size_t i = 0; i < eredeti->elemszam; i++) {
    unsigned char eredeti_ertek = eredeti->tomb[i];
    unsigned char norm_ertek = uj_ertekek->tomb[eredeti_ertek].elemszam;
    uj->tomb[i] = norm_ertek;
  }
}
void kiiras(t_kep *kep, char *fajlnev) {
  FILE *fp = fopen(fajlnev, "w");
  for (size_t i = 0; i < kep->elemszam; i++) {
    fprintf(fp, "%d\n", kep->tomb[i]);
  }
  fclose(fp);
}

void betolt_rgb(t_rgb *r, char *fajlnev) {
  int szelesseg = 0, magassag = 0;
  size_t idx = 0;
  for (idx = 0; idx < strlen(fajlnev) - 1; idx++) {
    if (fajlnev[idx] <= '9' && fajlnev[idx] >= '0' && fajlnev[idx + 1] == 'x') {
      idx++;
      break;
    }
  }
  int i = idx - 1;
  int j = idx + 1;
  while (fajlnev[i] <= '9' && fajlnev[i] >= '0') {
    i--;
  }
  while (fajlnev[j] <= '9' && fajlnev[j] >= '0') {
    j++;
  }
  for (size_t i_m = i + 1; i_m < idx; i_m++) {
    magassag *= 10;
    magassag += fajlnev[i_m] - 48;
  }
  for (size_t i_s = idx + 1; i_s < j; i_s++) {
    szelesseg *= 10;
    szelesseg += fajlnev[i_s] - 48;
  }

  printf("[ DEBUG ][ KEP ][ SZELESSEG ]:%d\n", szelesseg);
  printf("[ DEBUG ][ KEP ][ MAGASSAG ]:%d\n", magassag);

  r->szelesseg = szelesseg;
  r->magassag = magassag;
  r->elemszam = szelesseg * magassag;
  r->r = (unsigned char *)malloc(sizeof(unsigned char) * (szelesseg * magassag));
  r->g = (unsigned char *)malloc(sizeof(unsigned char) * (szelesseg * magassag));
  r->b = (unsigned char *)malloc(sizeof(unsigned char) * (szelesseg * magassag));

  FILE *fp = fopen(fajlnev, "r");
  if (NULL == fp)
    return;
  int _r, _g, _b;
  for (size_t i = 0; i < (szelesseg * magassag); i++) {
    fscanf(fp, " %d %d, %d", &_r, &_g, &_b);
    r->r[i] = (unsigned char) _r;
    r->g[i] = (unsigned char) _g;
    r->b[i] = (unsigned char) _b;
  }
  fclose(fp);
}
void kiir_rgb(t_rgb *r, char *fajlnev) {
  FILE *fp = fopen(fajlnev, "w");
  if ( NULL == fp)
    return;
  for(size_t i=0; i < r->elemszam; i++) {
    fprintf( fp, "%d %d %d\n", r->r[i], r->g[i], r->b[i]);
  }
  fclose(fp);
}
