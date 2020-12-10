#ifndef __COMMON_H__
#define __COMMON_H__

/* Uncomment these macros to enable corresponding functionality. */
/* pa3.1
 * 2020-12-10
 * defined the macro HAS_CTE to open init_irq() and _cte_init()
 */
#define HAS_CTE
//#define HAS_VME

#include <am.h>
#include <klib.h>
#include "debug.h"

typedef char bool;
#define true 1
#define false 0

#endif
