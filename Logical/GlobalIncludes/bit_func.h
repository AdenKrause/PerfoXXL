/****************************************************************************/
/*	Headermodul für Bitbehandlungsfunktionen								*/
/*																			*/
/*	Änderungsdatum:		15.03.00											*/
/*																			*/
/****************************************************************************/

#define BIT_SET(x,y) ( x | (1 << y))
#define BIT_CLR(x,y) ( x & ~(1 << y))
#define BIT_TST(x,y) ( (x & (1 << y)) != 0 ? 1 : 0)
#define NOT(x)		 (x == 0 ? 1 : 0)


