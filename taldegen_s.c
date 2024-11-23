/*  Konputagailuen Arkitektura - Informatika Ingeniaritza (IF - EHU)
    OpenMP laborategia - PROIEKTUA

    taldegen_s.c     SERIEKO BERTSIOA

    Informazio genetikoa prozesatzea eritasunei buruzko informazioa lortzeko.
    ALDAKOP aldagaiko elementuak, "taldekop" taldetan sailkatzeko, "distantzien" arabera

    Sarrera: dbgen.dat      informazio genetikoa duen fitxategia 
             dberi.dat      eritasunei buruzko informazioa duen fitxategia
    Irteera: emaitzak_s.out zentroideak, talde kopurua eta haien tamaina eta trinkotasuna
                    	    eritasunei buruzko informazioa

    funtg_s.c moduluarekin konpilatu;  -lm aukera gehitu
*****************************************************************************************/

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
    printf ("   Talde kopurua: %d -- Iterazio kopurua: %d\n", taldekop, iterkop);

    // B. Sailkatzearen "kalitatea"
    // ===========================

    for (i=0; i<taldekop; i++)  kideak[i].kop = 0;

    // Talde bakoitzeko elementuak (osagaiak) eta kopurua

    for (i=0; i<elekop; i++) 
    {
      taldea = sailka[i];
      zenb = kideak[taldea].kop;
      kideak[taldea].osagaiak[zenb] = i;
      kideak[taldea].kop ++; 
    }

    // Sailkatzearen balidazioa eta konbergentzia
    // OSATZEKO funtg_s fitxategian
    
    cvi = balidazioa (elem, kideak, zent, talde_trinko);

    dif = cvi - cvi_zaharra;
    if (dif < DELTA2) amaitu_sailkapena = 1;
    else {
      taldekop += 10;
      cvi_zaharra = cvi;
    }
  }

  clock_gettime (CLOCK_REALTIME, &t3);



  // 2. FASEA: analizatu eritasunak
  // ==============================

  // Taldeen analisia, eritasunak: medianen maximoa, minimoa...
  // OSATZEKO funtg_s fitxategian
  
  eritasunen_analisia (kideak, eri, eripro);

  clock_gettime (CLOCK_REALTIME, &t4);



  // Idatzi emaitzak emaitzak_s.out fitxategian
  // ==========================================
  
  f2 = fopen ("emaitzak_s.out", "w");
  if (f2 == NULL) {
    printf ("Errorea emaitzak_s.out fitxategia irekitzean\n");
    exit (-1);
  }
  
  fprintf (f2, " >> Taldeen zentroideak\n");
  for (i=0; i<taldekop; i++) {
    for (j=0; j<ALDAKOP; j++) fprintf (f2, "%7.3f", zent[i][j]);
    fprintf (f2,"\n");
  }

  fprintf (f2, "\n >> Taldeen tamaina: %d talde\n\n", taldekop);
  for (i=0; i<taldekop/10; i++) {
    for (j=0; j<10; j++) fprintf (f2, "%9d", kideak[10*i+j].kop);
    fprintf (f2, "\n");
  }
  for (i=i*10; i<taldekop; i++) fprintf (f2, "%9d", kideak[i].kop);
  fprintf (f2, "\n");

  fprintf (f2, "\n >> Taldeen trinkotasuna \n\n");
  for (i=0; i<taldekop/10; i++) {
    for (j=0; j<10; j++) fprintf (f2, "%9.2f", talde_trinko[10*i+j]);
    fprintf (f2, "\n");
  }
  for (i=i*10; i<taldekop; i++) fprintf (f2, "%9.2f", talde_trinko[i]);
  fprintf (f2, "\n");

  fprintf (f2, "\n >> Eritasunen analisia (medianak)\n");
  fprintf (f2, "\n Erit.  M_max - Tald   M_min - Tald");
  fprintf (f2, "\n ==================================\n");
  for (i=0; i<ERIMOTA; i++) 
    fprintf (f2, "  %2d     %4.2f - %2d      %4.2f - %2d\n", i, eripro[i].mmax, eripro[i].taldemax, eripro[i].mmin, eripro[i].taldemin);
  
  fclose (f2);

  clock_gettime (CLOCK_REALTIME, &t5);


  // Pantailaratu emaitza batzuk
  // ===========================

  t_irak   = (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / (double)1e9;
  t_sailka = (t3.tv_sec - t2.tv_sec) + (t3.tv_nsec - t2.tv_nsec) / (double)1e9;
  t_eri    = (t4.tv_sec - t3.tv_sec) + (t4.tv_nsec - t3.tv_nsec) / (double)1e9;
  t_idatzi = (t5.tv_sec - t4.tv_sec) + (t5.tv_nsec - t4.tv_nsec) / (double)1e9;

  printf ("\n   Exekuzio-denborak\n");
  printf ("\n   T_irakurtzea: %7.3f s", t_irak);
  printf ("\n   T_sailkatzea: %7.3f s", t_sailka);
  printf ("\n   T_eritasunak: %7.3f s", t_eri);
  printf ("\n   T_idaztea:    %7.3f s", t_idatzi);
  printf ("\n   ===================");
  printf ("\n        T_osoa:  %7.3f s\n\n", t_irak + t_sailka + t_eri + t_idatzi);
  
#ifndef ESPERI
  printf ("\n >> 0, 30 eta 60 zentroideak \n");
  for (j=0; j<ALDAKOP; j++) printf ("%7.3f", zent[0][j]);
  printf("\n");
  for (j=0; j<ALDAKOP; j++) printf ("%7.3f", zent[20][j]);
  printf("\n");
  for (j=0; j<ALDAKOP; j++) printf ("%7.3f", zent[40][j]);
  printf("\n");

  printf ("\n >> Taldeeen tamaina: %d talde \n\n", taldekop);
  for (i=0; i<taldekop/10; i++) {
    for (j=0; j<10; j++) printf ("%9d", kideak[10*i+j].kop);
    printf("\n");
  }
  for (i=i*10; i<taldekop; i++) printf ("%9d", kideak[i].kop);
  printf ("\n");

  printf ("\n >> Taldeen trinkotasuna \n\n");
  for (i=0; i<taldekop/10; i++) {
    for (j=0; j<10; j++) printf ("%9.2f", talde_trinko[10*i+j]);
    printf("\n");
  }
  for (i=i*10; i<taldekop; i++) printf ("%9.2f", talde_trinko[i]);
  printf ("\n");

  printf ("\n >> Eritasunen analisia (medianak)\n");
  printf ("\n Erit.  M_max - Tald   M_min - Tald");
  printf ("\n ==================================\n");
  for (i=0; i<ERIMOTA; i++)
    printf ("  %2d     %4.2f - %2d      %4.2f - %2d\n", i, eripro[i].mmax, eripro[i].taldemax, eripro[i].mmin, eripro[i].taldemin);
  printf("\n\n");
#endif
}
