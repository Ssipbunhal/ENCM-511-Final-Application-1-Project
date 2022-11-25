
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "IOs.h"

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */





#endif	/* XC_HEADER_TEMPLATE_H */

extern uint8_t CNflag; 
extern uint8_t minutes;
extern uint8_t seconds;
extern int countdown_start;
extern int countdown_pause;
extern int countdown_reset;
extern int alarm;
void IOinit(void);
void IOcheck(void);