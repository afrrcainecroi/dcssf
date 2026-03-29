#pragma once

//#ifdef	__cplusplus
//extern "C" {
//#endif

//Qui definisco quale versione di EIS sto generando!
#define EIS_VERSION "4.0"

#define EIS_LEVEL_1  0x0001 //Versione base
#define EIS_LEVEL_2  0x0002 //Versione con verifica unicità. Vuole cookie di sessione, cookie di identità e tempo
#define EIS_LEVEL_3  0x0004 //Versione che genera session id unici come cookie
#define EIS_LEVEL_4  0x0008 //Attiva data analytics
#define EIS_LEVEL_5  0x0010 //Attiva funzioni di anonimizzazione delle variabili inviate da client a server (GET oppure POST))
#define EIS_LEVEL_6  0x0020 //(to be done) attiva funzioni di anonimizzazione delle variabili inviate da server a client, per preparare GET o POST 
#define EIS_LEVEL_7  0x0040 //Attiva l'uso della manage. Tutto quello che è classificato come manage, lo passa al lisp
#define EIS_LEVEL_8  0x0080
#define EIS_LEVEL_9  0x0100
#define EIS_LEVEL_10 0x0200
#define EIS_LEVEL_11 0x0400
#define EIS_LEVEL_12 0x0800
#define EIS_LEVEL_13 0x1000
#define EIS_LEVEL_14 0x2000
#define EIS_LEVEL_15 0x4000
#define EIS_LEVEL_16 0x8000

#define VADEVEL 1

//Genera versione estesa
//#define EIS_LEVEL   (EIS_LEVEL_1 | EIS_LEVEL_2 | EIS_LEVEL_3 | EIS_LEVEL_4 | EIS_LEVEL_5 | EIS_LEVEL_7 | EIS_LEVEL_8 | EIS_LEVEL_9 | EIS_LEVEL_10 | EIS_LEVEL_11 | EIS_LEVEL_12 | EIS_LEVEL_13 | EIS_LEVEL_14 | EIS_LEVEL_15 | EIS_LEVEL_16 )

//Genera versione con solo eis versione base e LISP
//#undef EIS_LEVEL
#define EIS_LEVEL (EIS_LEVEL_1 | EIS_LEVEL_7)

//Genera versione con solo eis LISP
//#undef EIS_LEVEL
//#define EIS_LEVEL EIS_LEVEL_7

//#ifdef	__cplusplus
//}
//#endif

