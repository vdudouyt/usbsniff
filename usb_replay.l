%{
#include <stdlib.h>
#include "usb_replay.h"
#include "y.tab.h"
void yyerror(char *);
%}

%%

CTRL                   return CTRL;
BULK                   return BULK;
INTR                   return INTR;
ISOC                   return ISOC;
[0-9]+\.[0-9]+         return FLOAT;
IN                     return IN;
OUT                    return OUT;
[0-9a-f]+              return HEX;

[-+\n_()#:]            { return *yytext; }
[ \t]                  ; /* skip whitespace */

%%
