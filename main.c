#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Szurke kep tarolas.
 *
 * Egy dinamikus tombben tarolt kep struktura.
 * Konyebb igy kezelni mintha mindig csak tombkent latnank.
 */
typedef struct _s_kep {
  unsigned char *tomb; ///< Kep taroloeleme.
  size_t szelesseg;    ///< Kep szelessege.
  size_t magassag;     ///< Kep magassaga.
  size_t elemszam;     ///< Tomb elemszama.
} t_kep;
/** RGB kep tarolasa.
 * Lehetne akar tobb t_kep strukturat tarolni.
 */
typedef struct _s_rgb {
  unsigned char *r; ///< Voros kep tarolasa.
  unsigned char *g; ///< Zold elem tarolasa.
  unsigned char *b; ///< Kek elem tarolasa.
  size_t szelesseg; ///< Kep szelessege.
  size_t magassag;  ///< Kep magassaga.
  size_t elemszam;  ///< Tomb Kep elemszama.
} t_rgb;
/** Hisztogram elem.
 * Intervallum es elemszam paros.
 */
typedef struct _s_hist_elem {
  unsigned char intervallum; ///< Intervallum kozep erteke.
  size_t elemszam;           ///< Mennyi elemet tartalmaz.
} hist_elem;
/** Hisztogram struktura.
 * Bin-nek megfelelo elemszam.
 * Akar at is lehetne nevezni.
 */
typedef struct _s_hist {
  hist_elem *tomb; ///< Hiszt elemek tombje.
  size_t elemszam; ///< Bin-nyi elem.
} histogram;

/** Szurke kep betoltese fajlbol.
 *
 * A fuggveny a fajlnevbol automatikusan lekeri a kep nagysagat.
 * Megfelelo formatum lehet: \w+\d{1,4}x\d{1,4}.dat
 * A kepek numpy-bol dtype=uint8 exportalassal mentsuk.
 *
 * @param kep ahova beolvassuk.
 * @param fajlnev ahonnan beolvasunk.
 */
void betoltes(t_kep *, char *);
/** Szines kep betoltese fajlbol.
 *
 * A fuggveny a fajlnevbol automatikusan lekeri a kep nagysagat.
 * Megfelelo formatum lehet: \w+\d{1,4}x\d{1,4}.dat
 * A kepek numpy-bol dtype=uint8 exportalassal mentsuk.
 *
 * @param kep ahova beolvassuk.
 * @param fajlnev ahonnan beolvasunk.
 */
void betolt_rgb(t_rgb *, char *);
/** Hisztogram keszitse.
 *
 * @param kep amibol keszitjuk a hisztorgrammot.
 * @param hiszt amibe elkeszitjuk.
 * @param osztas mennyi egysegre osztunk.
 */
void hisztogram_keszit(t_kep *, histogram *, unsigned int);
/** Hisztogram kiirasa fajlba.
 *
 * @param hiszt amit kiirunk.
 * @param fajnev ahova kiirunk.
 * @param arg appendix a fajlnevhez.
 */
void histogram_kiir(histogram *, char *, char *);
/** Kumulativ eloszlas szamolasa.
 *
 * @param hiszt amibol keszitjuk.
 * @param eloszl amibe keszitjuk.
 */
void kumul_eloszlas(histogram *, histogram *);
/** Uj pixel ertekek szamolasa.
 *
 * @param eloszl kumulativ eloszlas.
 * @param uj uj ertekek, szinten hisztogram.
 * @param total elemszam, szelesseg x magassag.
 */
void uj_ertek(histogram *, histogram *, size_t);
/** Hisztogtam kiegyenlites a kepen az uj ertekek alapjan.
 *
 * @param eredeti eredeti kep.
 * @param uj uj kep ami normalizalt.
 * @param uj_ertekek uj pixel ertekek hisztogramja.
 */
void normalize(t_kep *, t_kep *, histogram *);
/** Szines kep normalizalasa.
 *
 * Fontos! Felulirja az eredeti kepet.
 * @param r kep.
 */
void normalize_RGB(t_rgb *);
/** Szurke kep kiirasa fajlba sorfolytonosan.
 *
 * @param kep kep amit kiirunk.
 * @param fajlnev.
 */
void kiiras(t_kep *, char *);
/** Szines kep kiirasa fajlba.
 *
 * @param kep.
 * @param fajlnev.
 */
void kiir_rgb(t_rgb *, char *);
/** Fajlnev vegehez ad egy szovegreszt.
 *
 * Segedfuggveny, hogy ne kelljen mindig uj fajlnevet begepelni.
 *
 * @param str fajlnev.
 * @param srg hozzakapcsolt szoveg.
 * @return fajlv hozzakapcsolt szoveggel.
 */
char *strAddExtension(char *, char *);

int main(int argc, char *argv[]) {
  if (argc < 2)
    return -1;
  char fajlnev[strlen(argv[1] + 1)];
  strcpy(fajlnev, argv[1]);

  t_kep kep, uj_kep;
  histogram hist_16, hist, uj_hist;
  histogram eloszl, uj_eloszl;
  histogram uj_ertekek;
  betoltes(&kep, fajlnev);

  hisztogram_keszit(&kep, &hist_16, 16);
  histogram_kiir(&hist_16, fajlnev, "_hist_16");

  hisztogram_keszit(&kep, &hist, 256);
  kumul_eloszlas(&hist, &eloszl);
  histogram_kiir(&hist, fajlnev, "_hist_256");
  histogram_kiir(&eloszl, fajlnev, "_kum_eredeti");

  uj_ertek(&eloszl, &uj_ertekek, kep.elemszam);
  histogram_kiir(&uj_ertekek, fajlnev, "_uj_ertekek");

  normalize(&kep, &uj_kep, &uj_ertekek);

  kiiras(&uj_kep, "fájlnév");

  hisztogram_keszit(&uj_kep, &uj_hist, 256);
  kumul_eloszlas(&uj_hist, &uj_eloszl);
  histogram_kiir(&uj_hist, fajlnev, "_hist_256_u");
  histogram_kiir(&uj_eloszl, fajlnev, "_kum_uj");

  if (argc == 3) {
    t_rgb rgb;
    betolt_rgb(&rgb, argv[2]);
    normalize_RGB(&rgb);
    kiir_rgb(&rgb, argv[2]);
    free(rgb.r);
    free(rgb.g);
    free(rgb.b);
  }

  free(hist.tomb);
  free(hist_16.tomb);
  free(uj_hist.tomb);
  free(eloszl.tomb);
  free(uj_eloszl.tomb);
  free(kep.tomb);
  free(uj_kep.tomb);
  free(uj_ertekek.tomb);
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

  /*printf("[ DEBUG ][ KEP ][ SZELESSEG ]:%d\n", szelesseg);*/
  /*printf("[ DEBUG ][ KEP ][ MAGASSAG ]:%d\n", magassag);*/
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
void histogram_kiir(histogram *hiszt, char *fajlnev, char *arg) {
  char *uj_fajl = strAddExtension(fajlnev, arg);
  FILE *fp = fopen(uj_fajl, "w");
  if (NULL == fp)
    return;
  for (size_t i = 0; i < hiszt->elemszam; i++) {
    fprintf(fp, "%d\t%ld\n", hiszt->tomb[i].intervallum,
            hiszt->tomb[i].elemszam);
  }
  fclose(fp);
  free(uj_fajl);
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

  /*printf("[ DEBUG ][ KEP ][ SZELESSEG ]:%d\n", szelesseg);*/
  /*printf("[ DEBUG ][ KEP ][ MAGASSAG ]:%d\n", magassag);*/

  r->szelesseg = szelesseg;
  r->magassag = magassag;
  r->elemszam = szelesseg * magassag;
  r->r =
      (unsigned char *)malloc(sizeof(unsigned char) * (szelesseg * magassag));
  r->g =
      (unsigned char *)malloc(sizeof(unsigned char) * (szelesseg * magassag));
  r->b =
      (unsigned char *)malloc(sizeof(unsigned char) * (szelesseg * magassag));

  FILE *fp = fopen(fajlnev, "r");
  if (NULL == fp)
    return;
  int _r, _g, _b;
  for (size_t i = 0; i < (szelesseg * magassag); i++) {
    fscanf(fp, "%d %d %d ", &_r, &_g, &_b);
    r->r[i] = (unsigned char)_r;
    r->g[i] = (unsigned char)_g;
    r->b[i] = (unsigned char)_b;
  }
  fclose(fp);
}
void kiir_rgb(t_rgb *r, char *fajlnev) {
  char *uj_fajl = strAddExtension(fajlnev, "_new");

  FILE *fp = fopen(uj_fajl, "w");
  if (NULL == fp)
    return;
  for (size_t i = 0; i < r->elemszam; i++) {
    fprintf(fp, " %d %d %d \n", r->r[i], r->g[i], r->b[i]);
  }
  fclose(fp);
  free(uj_fajl);
}
void normalize_RGB(t_rgb *r) {

  t_kep _r, _g, _b;
  _r.szelesseg = r->szelesseg;
  _r.magassag = r->magassag;
  _r.elemszam = r->elemszam;
  _r.tomb = (unsigned char *)malloc(sizeof(unsigned char) * _r.elemszam);
  _g.szelesseg = r->szelesseg;
  _g.magassag = r->magassag;
  _g.elemszam = r->elemszam;
  _g.tomb = (unsigned char *)malloc(sizeof(unsigned char) * _g.elemszam);
  _b.szelesseg = r->szelesseg;
  _b.magassag = r->magassag;
  _b.elemszam = r->elemszam;
  _b.tomb = (unsigned char *)malloc(sizeof(unsigned char) * _b.elemszam);

  for (size_t i = 0; i < r->elemszam; i++) {
    _r.tomb[i] = r->r[i];
    _g.tomb[i] = r->g[i];
    _b.tomb[i] = r->b[i];
  }

  histogram h_r, h_g, h_b;
  hisztogram_keszit(&_r, &h_r, 256);
  hisztogram_keszit(&_g, &h_g, 256);
  hisztogram_keszit(&_b, &h_b, 256);

  histogram k_r, k_g, k_b;
  histogram u_r, u_g, u_b;

  kumul_eloszlas(&h_r, &k_r);
  kumul_eloszlas(&h_g, &k_g);
  kumul_eloszlas(&h_b, &k_b);

  uj_ertek(&k_r, &u_r, r->elemszam);
  uj_ertek(&k_g, &u_g, r->elemszam);
  uj_ertek(&k_b, &u_b, r->elemszam);

  free(_r.tomb);
  free(_g.tomb);
  free(_b.tomb);
  free(h_r.tomb);
  free(h_g.tomb);
  free(h_b.tomb);
  free(k_r.tomb);
  free(k_g.tomb);
  free(k_b.tomb);

  for (size_t i = 0; i < r->elemszam; i++) {
    r->r[i] = u_r.tomb[r->r[i]].elemszam;
    r->g[i] = u_g.tomb[r->g[i]].elemszam;
    r->b[i] = u_b.tomb[r->b[i]].elemszam;
  }
  free(u_r.tomb);
  free(u_g.tomb);
  free(u_b.tomb);
}
char *strAddExtension(char *str, char *arg) {
  size_t str_idx = strlen(str);
  char *uj_fajl = (char *)malloc(sizeof(char) * (str_idx + strlen(arg) + 1));
  strncpy(uj_fajl, str, str_idx - 4);
  uj_fajl[str_idx - 4] = '\0';
  strcat(uj_fajl, arg);
  strcat(uj_fajl, ".dat");
  /*printf("[ DEBUG ][ KIIR ][ NEW ] %s\n", uj_fajl);*/
  return uj_fajl;
}
