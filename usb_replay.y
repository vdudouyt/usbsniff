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

packet_body:
        | CTRL '_' OUT endpoint ':' HEX ':' HEX ':' HEX ':' HEX ':' HEX ':' HEX
        | CTRL '_' IN  endpoint ':' HEX
        | non_ctrl '_' direction endpoint ':' HEX
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
