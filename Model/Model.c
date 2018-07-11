#include <stdio.h>
#include <stdlib.h>
#include "Kalman.h"
#include "PID.h"
#include <math.h>

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
model_st_t model_st={25, -5, 315,1, 1500, 400, 0.085};
model_st_t* m = &model_st;
/*=====================================================================================================*/
int main()
{
	/*Utworzenie pliku na wyniki*/
	FILE * fp;
	fp = fopen("dane.txt", "w+");

	/* Zmienne */
	float Zs; /* Zysk temperatury wewnętrznej z grzałki */
	float dT; /* Różnica temperatur */
	float Ptot; /* Strumień ciepła do zewnątrz */
	float P = 0; /* Aktualne ciepło grzałki */
	float R = pow(m->U, -1);
	float Tpop;
	float mia;
	/*Pętla główna*/
	int i; for(i = 0; i<36000; i++)/* t [s] */
	{
		//if(i>2400){m->Tz=10;};

		if(m->G == 1)/* Włączenie grzałki -> P(t) = Pb*exp(t/(R*C)) */
				{
					if (P >= m->Pg){/*Wartość już nie rośnie*/}else{P = m->Pg * exp(-i/(R*m->C));}/* Nagrzewanie grzałki */
					dT = abs(m->Tw - m->Tz); /* Ustalenie różnicy temperatur */
					Ptot = m->U * m->Pow * dT;/* Ciepło uchodzące z budynku */
					Zs = (P-Ptot) / m->C;		/* temperatura zapewniona przez ciepło grzałki */
					Tpop = m->Tw;
					m->Tw = m->Tw + Zs; /* Zmiana temperatury wewnątrz */
					m->G = PID(Tpop, m->Tw);
				}
				else/* G != 1 */
				{
					if (P <= 0.1){/*Wartość już nie maleje*/}else{P = m->Pg *(1 - exp(-i/(R*m->C)));}/* Ochładzanie grzałki */
					dT = abs(m->Tw - m->Tz); /* Ustalenie różnicy temperatur */
					Ptot = m->U * m->Pow * dT;/* Ciepło uchodzące z budynku */
					Zs = (P-Ptot) / m->C;		/* temperatura zapewniona przez ciepło grzałki */
					Tpop = m->Tw;
					m->Tw = m->Tw + Zs; /* Zmiana temperatury wewnątrz */
					m->G = PID(Tpop, m->Tw);
				}
		 fprintf(fp, "%d %f\n",i, m->Tw);
	}
	fclose(fp);
	system("gnuplot Rysowanie.gp -p");
	return 0;
}



//typedef struct {
	//int U;
	//int dt;
	//int K_p;
	//int K_i;
	//int K_d;
	//int out;
	//int integral;
//}PID_st_t;
PID_st_t PID_st={0, 1, 8, 20, 1.5, 1, 1};
PID_st_t* PP_PID=&PID_st;

float PID(float Tpop, float Tw)
{
int pre_error = Tpop - Tw;
//int pre_error =0;
//int i;
//for(i = 0; i < 2; i++)
//{


PP_PID->integral = (Tpop - Tw) * PP_PID->dt;

PP_PID->U =  PP_PID->K_p * Tpop - Tw
				+
				PP_PID->K_i * PP_PID->integral
				+
				(((Tpop - Tw) - pre_error) * PP_PID->K_d) / PP_PID->dt;


//}
int G;

if (PP_PID->U < -2)
{
	G = 1;
}
else if(PP_PID->U >2)
{
	G = 0;
}

return G;
}
