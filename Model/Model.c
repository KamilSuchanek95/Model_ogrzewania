#include <stdio.h>
#include <stdlib.h>
#include "Kalman.h"
#include "PID.h"

/*=====================================================================================================*/
/*Parametry początkowe:*/
typedef struct{
	float Tw;   /*Temperatura wewnętrzna budynku => [ *C ]*/
	float Tz;   /*Temperatura na zewnątrz budynku => [ *C ]*/
	float C; 	/*Pojemność cieplna budynku => [ J/K = (W*s)/K ]*/
	int G;      /*Włączenie grzałki 0/1 => [ boolean ]*/
	float Pg;	/*Moc grzałki => [ W ]*/
	float Pow;	/*Powierzchnia umowna budynku => [ m^2 ]*/
	float U; 	/*Przenikalność temperaturowa budynku => [ W/(m^2 * K) ]*/
}model_st_t;
model_st_t model_st={25, -5, 3150,1, 1500,400,0.085};
model_st_t* m = &model_st;
/*=====================================================================================================*/
int main(){
	/*Utworzenie pliku na wyniki*/
	FILE * fp;
	fp = fopen("dane.txt", "w+");

	/* Zmienne */
	float Zs; /* Zysk temperatury wewnętrznej z grzałki */
	float dT; /* Różnica temperatur */
	float Ptot; /* Strumień ciepła do zewnątrz */
	float P = 0; /* Aktualne ciepło grzałki */
	float R = pow(m->U, -1);
	/*Pętla główna*/
	int i; for(i = 0; i<3600; i++)/* t */
	{
		if(i>2400){m->G=0;};

		if(m->G == 1)/* Włączenie grzałki -> P(t) = Pb*exp(t/(R*C)) */
				{
					if (P >= m->Pg){/*Wartość już nie rośnie*/}else{P = m->Pg * exp(i/(R*m->C));}/* Nagrzewanie grzałki */
					dT = abs(m->Tw - m->Tz); /* Ustalenie różnicy temperatur */
					Ptot = m->U * m->Pow * dT;/* Ciepło uchodzące z budynku */
					Zs = (P-Ptot) / m->C;		/* temperatura zapewniona przez ciepło grzałki */
					m->Tw = m->Tw + Zs; /* Zmiana temperatury wewnątrz */
				}
				else/* G != 1 */
				{
					if (P <= 0.1){/*Wartość już nie maleje*/}else{P = m->Pg *(1 - exp(-i/(R*m->C)));}/* Ochładzanie grzałki */
					dT = abs(m->Tw - m->Tz); /* Ustalenie różnicy temperatur */
					Ptot = m->U * m->Pow * dT;/* Ciepło uchodzące z budynku */
					Zs = (P-Ptot) / m->C;		/* temperatura zapewniona przez ciepło grzałki */
					m->Tw = m->Tw + Zs; /* Zmiana temperatury wewnątrz */
				}
		 fprintf(fp, "%d %f\n",i, m->Tw);
	}
	fclose(fp);
	system("gnuplot Rysowanie.gp -p");
	return 0;
}



