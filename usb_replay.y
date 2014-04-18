%{
    #include <stdio.h>
    int yylex(void);
    void yyerror(char *);
%}

%token CTRL BULK INTR ISOC OUT IN HEX FLOAT

%%

document:
        document packet '\n'    { printf("packet\n"); }
        |
        ;

direction: OUT | IN

endpoint:       '(' FLOAT ')'
timestamp:      '#' FLOAT
non_ctrl:       BULK | INTR | ISOC

bmRequestType:  HEX
bRequest:       HEX
wValue:         HEX
wIndex:         HEX
wLength:        HEX
data:           HEX

packet_body:
        | CTRL '_' OUT endpoint ':' bmRequestType ':' bRequest ':' wValue ':' wIndex ':' wLength ':' data
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
    yyparse();
    return 0;
}
