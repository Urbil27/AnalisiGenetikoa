/*  Konputagailuen Arkitektura - Informatika Ingeniaritza (IF - EHU)
    OpenMP laborategia - PROIEKTUA

    funtg.h
    taldegen fitxategian erabiltzen diren funtzioen goiburukoak
*******************************************************************/

extern void   hasierako_zentroideak 
              	(float zent[][ALDAKOP]);

extern int    zentroide_berriak     
		(float elem[][ALDAKOP], 
		 float zent[][ALDAKOP], 
		 int   *sailka, 
		 int   elekop
		);

extern double distantzia_genetikoa  
		(float *elem1, 
		 float *elem2
		);

extern void   talde_gertuena
	        (int   elekop, 
	         float elem[][ALDAKOP], 
	         float zent[][ALDAKOP], 
	         int   *sailka
	        );

extern double balidazioa
            	(float  elem[][ALDAKOP], 
 	         struct taldeinfo *kideak, 
		 float  zent[][ALDAKOP], 
		 float  *talde_trinko
		);

extern void   eritasunen_analisia 
        	(struct taldeinfo *kideak, 
		 float  eri[][ERIMOTA], 
		 struct analisia *eripro
		);

