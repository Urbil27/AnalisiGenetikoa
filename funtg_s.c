/*  Konputagailuen Arkitektura - Informatika Ingeniaritza (IF - EHU)
    OpenMP laborategia - PROIEKTUA

    funtg_s.c
    taldegen_s.c programan erabiltzen diren errutinak

    OSATZEKO
******************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h> // DBL_MAX
#include <float.h>

#include "definetg.h" // konstante eta datu-egituren definizioak

/* 1 - Bi elementuren arteko distantzia genetikoa kalkulatzeko funtzioa
       (distantzia euklidearra)

       Sarrera:  ALDAKOP aldagaiko bi elementu (erreferentziaz)
       Irteera:  distantzia (double)
******************************************************************************************/

double distantzia_genetikoa(float *elem1, float *elem2)
{
  // EGITEKO
  // Kalkulatu bi elementuren arteko distantzia (euklidearra)
  double distantzia = 0;
  for (int i; i < ALDAKOP; i++)
  {
    distantzia += pow(elem1[i] - elem2[i], 2);
  }
  return sqrt(distantzia);
}

/* 2 - Talde gertuena kalkulatzeko funtzioa (zentroide gertuena)

       Sarrera:  elekop   elementu kopurua, int
                 elem     EMAX x ALDAKOP tamainako matrizea
                 zent     taldekop x ALDAKOP tamainako matrizea
       Irteera:  sailka   EMAX tamainako bektorea, elementu bakoitzari dagokion taldea
******************************************************************************************/

void talde_gertuena(int elekop, float elem[][ALDAKOP], float zent[][ALDAKOP], int *sailka)
{
  // EGITEKO
  // sailka: elementu bakoitzaren zentroide hurbilena, haren "taldea"
  double distantzia_minimoa, uneko_distantzia;
  int gertuen_talde;
  for (int i = 0; i < elekop; i++)
  {
    distantzia_minimoa = DBL_MAX;
    gertuen_talde = -1;
    for (int j = 0; i < taldekop; j++)
    {
      uneko_distantzia = distantzia_genetikoa(elem[i], zent[j]);
      if (uneko_distantzia < distantzia_minimoa)
      {
        distantzia_minimoa = uneko_distantzia;
        gertuen_talde = j;
      }
    }
    sailka[i] = gertuen_talde;
  }
}

/* 3 - Sailkapenaren balidazioa: taldeen trinkotasuna eta zentroideen trinkotasuna
       cvi indizea kalkulatzen da

       Sarrera:  elem     fitxategiko elementuak (EMAX x ALDAKOP tamainako matrizea)
                 kideak   taldekideen zerrenda (taldekop tamainako struct-bektore bat: elem eta kop)
                 zent     taldeen zentroideak (taldekop x ALDAKOP)
       Irteera:  cvi indizea
                 talde_trinko    taldeen trinkotasuna
******************************************************************************************/

double balidazioa(float elem[][ALDAKOP], struct taldeinfo *kideak, float zent[][ALDAKOP], float *talde_trinko)
{

  // EGITEKO

  // Kalkulatu taldeen trinkotasuna: kideen arteko distantzien batezbestekoa

  // Kalkulatu zentroideen trinkotasuna: zentroide bakoitzeko, besteekiko b.b.-ko distantzia

  // Kalkulatu cvi indizea
  double distantzien_batura, cvi_batukaria;
  int a[taldekop];
  int b[taldekop];

  for (int i = 0; i < taldekop; i++)
  {
    distantzien_batura = 0;
    for (int j = 0; j < kideak[i].kop; j++)
    {
      for (int k = 0; k < kideak[i].kop; k++)
      {
        distantzien_batura += distantzia_genetikoa(kideak[i].osagaiak[j], kideak[i].osagaiak[k]);
      }
    }
    a[i] = distantzien_batura / kideak[i].kop - 1;
  }

  for (int i = 0; i < taldekop; i++)
  {
    distantzien_batura = 0;
    for (int j = 0; j < taldekop; j++)
    {
      distantzien_batura += distantzia_genetikoa(zent[i], zent[j]);
    }
    b[i] = distantzien_batura / taldekop - 1;
  }

  for (int i = 0; i < taldekop; i++)
  {
    cvi_batukaria += (b[i] - a[i]) / fmax(a[i], b[i]);
  }

  return cvi_batukaria / taldekop;
}



void burbuila_ordenazioa(float *bek[])
{
  int lag;
  for (int goi=sizeof(bek); goi>0; goi--)
  {
    for (int i=0; i<goi; i++)
    {
      if (bek[i] > bek[i+1])
      {
        lag = bek[i];
        bek[i] = bek[i+1];
        bek[i+1] = lag;
      }
    }
  }
}



/* 4 - Eritasunak analizatzeko funtzioa

       Sarrera:  kideak  taldekideen zerrenda (taldekop tamainako struct-bektore bat: elem eta kop)
                 eri     eritasunei buruzko informazioa (EMAX x ERIMOTA)
       Irteera:  eripro  eritasunen analisia: medianen maximoa/minimoa, eta taldeak
******************************************************************************************/

void eritasunen_analisia(struct taldeinfo *kideak, float eri[][ERIMOTA], struct analisia *eripro)
{

  // EGITEKO
  // Prozesatu eritasunei buruzko informazioa talde bakoitzeko kideen artean:
  // eritasunak agertzeko probabilitateen mediana.
  // Eritasun bakoitzerako, medianen maximoa eta minimoa eta zein taldetan.
  float mediana[ERIMOTA], taldeen_medianak[taldekop];
  for(int i=0;i<ERIMOTA;i++)
  {
    burbuila_ordenazioa(eri[i]);
    mediana[i]=eri[i][ERIMOTA/2];
  }

  burbuila_ordenazioa(mediana);
  eripro->mmax=mediana[ERIMOTA-1];
  eripro->mmin=mediana[0];

  for(int i = 0 ; i<taldekop; i++){
    for(int j = 0; j<kideak[i].kop;j++){
      burbuila_ordenazioa(kideak[i].osagaiak);
      taldeen_medianak[i]=kideak[i].osagaiak[kideak[i].kop/2];
    }
  }
  burbuila_ordenazioa(taldeen_medianak);
  eripro->taldemax=taldeen_medianak[taldekop-1];
  eripro->taldemin=taldeen_medianak[0];
}

// PROGRAMA NAGUSIAREN BESTE BI FUNTZIO
// ====================================

/* 5 - Zentroideen hasierako balioak
**************************************************************/

void hasierako_zentroideak(float zent[][ALDAKOP])
{
  int i, j;

  srand(147);

  for (i = 0; i < taldekop; i++)
    for (j = 0; j < ALDAKOP / 2; j++)
    {
      zent[i][j] = (rand() % 10000) / 100.0;
      zent[i][j + ALDAKOP / 2] = zent[i][j];
    }
}

/* 6 - Zentroide berriak; erabaki bukatu behar den edo ez
**************************************************************/

int zentroide_berriak(float elem[][ALDAKOP], float zent[][ALDAKOP], int *sailka, int elekop)
{
  int i, j, bukatu;
  float zentberri[taldekop][ALDAKOP];
  double diszent;
  double baturak[taldekop][ALDAKOP + 1];

  bukatu = 1; // 1: simulazioa bukatu da; 0: jarraitu behar da

  // Zentroide berriak: kideen dimentsio bakoitzeko aldagaien batezbestekoak
  // sailka: elementu bakoitzaren taldea
  // baturak: taldekideen aldagaien balioak batzeko; azkena kopurua da

  for (i = 0; i < taldekop; i++)
    for (j = 0; j < ALDAKOP + 1; j++)
      baturak[i][j] = 0.0;

  for (i = 0; i < elekop; i++)
  {
    for (j = 0; j < ALDAKOP; j++)
      baturak[sailka[i]][j] += elem[i][j]; // batu taldekideen balioak aldagai bakoitzeko

    baturak[sailka[i]][ALDAKOP]++; // azken osagaia (ALDAKOP) kopuruak (zenbat) gordetzeko
  }

  // Kalkulatu taldeetako zentroide berriak, eta erabaki bukatu behar den (DELTA1)

  for (i = 0; i < taldekop; i++)
  {
    if (baturak[i][ALDAKOP] > 0) // taldea ez dago hutsik
    {
      // Zentroide berrien "koordenatuak": kideen batezbestekoak
      for (j = 0; j < ALDAKOP; j++)
        zentberri[i][j] = baturak[i][j] / baturak[i][ALDAKOP];

      // Erabaki bukatu behar den
      diszent = distantzia_genetikoa(&zentberri[i][0], &zent[i][0]);
      if (diszent > DELTA1)
        bukatu = 0; // taldeko zentroidea mugitu da DELTA1 baino gehiago; segi simulazioarekin

      // Kopiatu zentroide berria
      for (j = 0; j < ALDAKOP; j++)
        zent[i][j] = zentberri[i][j];
    }
  }

  return (bukatu);
}
