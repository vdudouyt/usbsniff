%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "usb_replay.h"
    #include "error.h"
    int yylex(void);
    void yyerror(char *);
    extern char *yytext;
    #define YYSTYPE urb_t *
%}

%token CTRL BULK INTR ISOC OUT IN HEX FLOAT

%%

document:
    document packet '\n'    { perform_transfer(yylval); }
    |
    ;

direction:
    | OUT { yylval->direction = OUT; }
    | IN { yylval->direction = IN; }

endpoint:       '('
                FLOAT { sscanf(yytext, "%d.%d", &(yylval->device_address), &(yylval->endpoint)); }
                ')' 
timestamp:      '#' FLOAT
non_ctrl:
    | BULK { yylval->type = BULK; }
    | INTR { yylval->type = INTR; }
    | ISOC { yylval->type = ISOC; }

bmRequestType:  HEX { yylval->bmRequestType = hex_to_int(yytext); }
bRequest:       HEX { yylval->bRequest = hex_to_int(yytext); }
wValue:     HEX { yylval->wValue = hex_to_int(yytext); }
wIndex:     HEX { yylval->wIndex = hex_to_int(yytext); }
data:       HEX { hex_to_buf(yytext, yylval->data); yylval->data_size = strlen(yytext) / 2; }

packet_body:
    | CTRL '_' direction endpoint ':' bmRequestType ':' bRequest ':' wValue ':' wIndex ':' data
      {
        yylval->type = CTRL;
      }
    | non_ctrl '_' direction endpoint ':' data
    ;

packet: packet_body { yylval->timing = 0; }
    | packet_body timestamp { sscanf(yytext, "%Lf", &(yylval->timing)); }
    ;

%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int main(int argc, char **argv) {
    /* Parsing args */
    if(argc != 3) {
        print_help_and_exit(argv);
    }
    int vid, pid;
    if(!sscanf(argv[1], "%x:%x", &vid, &pid)) {
        ERROR("Couldn't parse vid:pid");
    }

    /* Initializations */
    if(!freopen(argv[2], "r", stdin)) {
        PERROR("Couldn't open file\n");
    }
    if(!(yylval = malloc(sizeof(urb_t)))) {
        ERROR("Couldn't malloc");
    }
    usb_init(vid, pid);

    printf("Sending traffic... ");
    fflush(stdout);
    yyparse();
    printf("\n");

    /* Dismiss */
    free(yylval);
    fclose(stdin);
    return 0;
}
