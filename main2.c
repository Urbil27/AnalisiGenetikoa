#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "definetg.h"			// konstante eta datu-egituren definizioak
#include "funtg.h"			// programan deitzen diren funtzioak


float             elem[EMAX][ALDAKOP];	// prozesatu behar diren elementuak (dbgen.dat fitxategian)
struct taldeinfo  kideak[TALDEKOPMAX];	// talde bakoitzeko kideen zerrenda

float            eri[EMAX][ERIMOTA];	// elementuen eritasunei buruzko datuak (dberi.dat fitxategian)
struct analisia  eripro[ERIMOTA];	// eritasunen probabilitateak: medianen maximoa, minimoa...


int  taldekop = 30;			// hasierako talde kopurua


// ================================
//       PROGRAMA NAGUSIA
// ================================

void main (int argc, char *argv[])
{
  float   zent[TALDEKOPMAX][ALDAKOP], zentberri[TALDEKOPMAX][ALDAKOP];   // zentroideak (taldeak)
  float   talde_trinko[TALDEKOPMAX];	// talde bakoitzeko trinkotasuna

  int     i, j, elekop, taldea, zenb;
  int     sailka[EMAX];			// elementu bakoitzeko taldea  
  int     bukatu = 0, iterkop = 0, amaitu_sailkapena; 
  double  cvi, cvi_zaharra, dif;

  FILE    *f1, *f2;
  struct timespec  t1, t2, t3, t4, t5;
  double  t_irak, t_sailka, t_eri, t_idatzi;



  if ((argc < 3) || (argc > 4)) {
    printf ("ADI:  progr - f1 (elem) - f2 (eri) - [elem kop])\n");
    exit (-1);
  }

  printf ("\n >> Exekuzioa seriean\n\n");
  clock_gettime (CLOCK_REALTIME, &t1);

  
  // Irakurri datuak sarrea-fitxategietatik
  // ======================================
  
  // Datu genetikoak (dbgen) >> elem[i][j]
  
  f1 = fopen (argv[1], "r");
  if (f1 == NULL) {
    printf ("Errorea %s fitxategia irekitzean\n", argv[1]);
    exit (-1);
  }

  fscanf (f1, "%d", &elekop);
  if (argc == 4) elekop = atoi (argv[3]);	// 4. parametroa: prozesatu behar diren elementuen kopurua

  for (i=0; i<elekop; i++) 
  for (j=0; j<ALDAKOP; j++) 
    fscanf (f1, "%f", &(elem[i][j]));		// elementuen zerrenda, ALDAKOP aldagaikoak
   
  fclose (f1);

  // Eritasunei buruzko informazioa (dberi) >> eri[i][j]

  f1 = fopen (argv[2], "r");
  if (f1 == NULL) {
    printf ("Errorea %s fitxategia irekitzean\n", argv[2]);
    exit (-1);
  }

  for (i=0; i<elekop; i++) 
  for (j=0; j<ERIMOTA; j++) 
    fscanf (f1, "%f", &(eri[i][j]));		// eritasunei buruzko informazioa
   
  fclose (f1);

  clock_gettime (CLOCK_REALTIME, &t2);


  // 1. FASEA: prozesu iteratiboa elementuak taldekop taldetan sailkatzeko
  // CVI indizean DELTA2 balioa baino diferentzia txikiagoa lortu arte.
  // =====================================================================

  amaitu_sailkapena = 0; 
  cvi_zaharra = -1;

  while (taldekop < TALDEKOPMAX && amaitu_sailkapena == 0)
  {
    // Aukeratu lehen zentroideak, ausaz
 
    hasierako_zentroideak (zent);

    // A. sailkatze-prozesua, taldekop taldetan
    // ========================================

    iterkop = 0; 
    bukatu = 0;

    while ((bukatu == 0) && (iterkop < ITMAX))
    {
      // Kalkulatu talde gertuena
      // OSATZEKO funtg_s fitxategian
  
      talde_gertuena (elekop, elem, zent, sailka);

      // Kalkulatu taldeetako zentroide berriak: kideen dimentsio bakoitzaren batezbestekoa
      // Aldaketa zentroideetan DELTA1 baino txikiagoa bada, bukatu = 1

      bukatu = zentroide_berriak (elem, zent, sailka, elekop);
  
      iterkop ++;
    }
  }

}