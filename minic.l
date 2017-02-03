%option noyywrap
%%
"+"|"-"|"*"|"/"|"("|")"|"="|";"|"{"|"}"|"["|"]"|"," { return yytext[0];}{printf("element is %d",yytext[0]);}
[ \t\n\r]* {}
"<=" {return LE;}
">=" {return GE;}
">" {return GT;}
"<" {return LT;}
"==" {return EQ;}
"!=" {return NE;}
"if" {return IF;}
"while" {return WHILE;}
"do" {return DO;}
"endif" {return ENDIF;}
"else" {return ELSE;}
"then" {return THEN;}
"print" {return PRINT;}
"enddo" {return ENDDO;}
"int" {return INT;}
[0-9]+ {yylval.pnode = new_ast_node(AST_NUMBER);
        yylval.pnode->ival=atoi(yytext);
        return NUMBER;}
[a-zA-Z]+[a-z0-9]* {printf("returning %s\n", yytext);
                    yylval.pnode = new_ast_node(AST_ID);
                    yylval.pnode->psym=getsym(yytext);
                    varname = strdup(yytext);
                   return ID;
	  }

%% 
