%{
    #include <stdio.h>
    #include "usb_replay.h"
    int yylex(void);
    void yyerror(char *);
    extern char *yytext;
    #define YYSTYPE urb_t *
%}

%token CTRL BULK INTR ISOC OUT IN HEX FLOAT

%%

document:
        document packet '\n'    { }
        |
        ;

direction: OUT | IN

endpoint:       '(' FLOAT ')'
timestamp:      '#' FLOAT
non_ctrl:       BULK | INTR | ISOC

bmRequestType:  HEX { hex_to_buf(yytext, yylval->bmRequestType); }
bRequest:       HEX { hex_to_buf(yytext, yylval->bRequest); }
wValue:         HEX { hex_to_buf(yytext, yylval->wValue); }
wIndex:         HEX { hex_to_buf(yytext, yylval->wIndex); }
wLength:        HEX { hex_to_buf(yytext, yylval->wLength); }
data:           HEX { hex_to_buf(yytext, yylval->data); }

packet_body:
        | CTRL '_' OUT endpoint ':' bmRequestType ':' bRequest ':' wValue ':' wIndex ':' wLength ':' data
          {
          }
        | CTRL '_' IN  endpoint ':' data
        | non_ctrl '_' direction endpoint ':' data
        ;

packet: packet_body
        | packet_body timestamp
        ;

%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int main(void) {
    yylval = malloc(sizeof(urb_t));
    yyparse();
    free(yylval);
    return 0;
}
