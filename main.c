/* main.c - Reconhecedor da linguagem miniPascal
   Trabalho Final de LFA - UDESC

   Fase 1: analise lexica com o flex (lex.yy.c), que separa o arquivo em tokens.
   Fase 2: automato de pilha (AP) construido a partir da gramatica livre de
           contexto. O AP e deterministico porque olha o proximo token e
           consulta a tabela LL(1) para escolher qual producao empilhar.

   Uso:  main teste.pas        (mostra todos os passos)
         main teste.pas -r     (mostra so o resultado)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "lex.yy.c"

/* ---------------- Nao-terminais (a partir de 100) ---------------- */

enum {
    PROGRAMA = 100, CABECALHO, LISTA_DECL, DECL, DECL_VAR, LISTA_IDS, RESTO_IDS,
    TIPO, PROCEDIMENTO, FUNCAO, PARAMS, LISTA_PARAMS, RESTO_PARAMS, PARAM,
    CORPO, SECAO_VAR, RESTO_VAR, BLOCO, LISTA_CMD, RESTO_CMD, CMD, RESTO_ID,
    SENAO, LISTA_ARGS, RESTO_ARGS, EXPR, RESTO_EXPR, EXPR_SIMPLES, RESTO_SOMA,
    TERMO, RESTO_MULT, FATOR, CHAMADA, PRINCIPAL
};

char *nome_terminal[NUM_TERMINAIS] = {
    "$", "program", "var", "procedure", "function", "begin", "end", "if",
    "then", "else", "while", "do", "integer", "real", "boolean", "and", "or",
    "not", "true", "false", "id", "num_int", "num_real", ":=", "oprel",
    "opsoma", "opmult", ";", ":", ",", "(", ")", "."
};

char *nome_nao_terminal[] = {
    "<programa>", "<cabecalho>", "<lista_decl>", "<decl>", "<decl_var>",
    "<lista_ids>", "<resto_ids>", "<tipo>", "<procedimento>", "<funcao>",
    "<params>", "<lista_params>", "<resto_params>", "<param>", "<corpo>",
    "<secao_var>", "<resto_var>", "<bloco>", "<lista_cmd>", "<resto_cmd>",
    "<cmd>", "<resto_id>", "<senao>", "<lista_args>", "<resto_args>", "<expr>",
    "<resto_expr>", "<expr_simples>", "<resto_soma>", "<termo>", "<resto_mult>",
    "<fator>", "<chamada>", "<principal>"
};

char *nome_simbolo(int s)
{
    if (s >= 100)
        return nome_nao_terminal[s - 100];
    return nome_terminal[s];
}

int eh_terminal(int s)
{
    return s < 100;
}

/* ---------------- Producoes da gramatica ----------------
   prod[n][0] = lado esquerdo, depois o lado direito, terminado em FIM.
   Producao vazia (epsilon) tem so o lado esquerdo. */

#define FIM -1
#define NUM_PRODUCOES 69

int prod[NUM_PRODUCOES + 1][10] = {
    {FIM},                                                      /* 0 nao usada */
    {PROGRAMA, CABECALHO, LISTA_DECL, PRINCIPAL, FIM},          /* 1 */
    {CABECALHO, T_PROGRAM, T_ID, T_PONTO_VIRG, FIM},            /* 2 */
    {CABECALHO, FIM},                                           /* 3 */
    {LISTA_DECL, DECL, LISTA_DECL, FIM},                        /* 4 */
    {LISTA_DECL, FIM},                                          /* 5 */
    {DECL, T_VAR, DECL_VAR, FIM},                               /* 6 */
    {DECL, DECL_VAR, FIM},                                      /* 7 */
    {DECL, PROCEDIMENTO, FIM},                                  /* 8 */
    {DECL, FUNCAO, FIM},                                        /* 9 */
    {DECL_VAR, LISTA_IDS, T_DOIS_PONTOS, TIPO, T_PONTO_VIRG, FIM}, /* 10 */
    {LISTA_IDS, T_ID, RESTO_IDS, FIM},                          /* 11 */
    {RESTO_IDS, T_VIRGULA, T_ID, RESTO_IDS, FIM},               /* 12 */
    {RESTO_IDS, FIM},                                           /* 13 */
    {TIPO, T_INTEGER, FIM},                                     /* 14 */
    {TIPO, T_REAL, FIM},                                        /* 15 */
    {TIPO, T_BOOLEAN, FIM},                                     /* 16 */
    {PROCEDIMENTO, T_PROCEDURE, T_ID, PARAMS, T_PONTO_VIRG, CORPO, T_PONTO_VIRG, FIM}, /* 17 */
    {FUNCAO, T_FUNCTION, T_ID, PARAMS, T_DOIS_PONTOS, TIPO, T_PONTO_VIRG, CORPO, T_PONTO_VIRG, FIM}, /* 18 */
    {PARAMS, T_ABRE_PAR, LISTA_PARAMS, T_FECHA_PAR, FIM},       /* 19 */
    {PARAMS, FIM},                                              /* 20 */
    {LISTA_PARAMS, PARAM, RESTO_PARAMS, FIM},                   /* 21 */
    {RESTO_PARAMS, T_PONTO_VIRG, PARAM, RESTO_PARAMS, FIM},     /* 22 */
    {RESTO_PARAMS, FIM},                                        /* 23 */
    {PARAM, LISTA_IDS, T_DOIS_PONTOS, TIPO, FIM},               /* 24 */
    {CORPO, SECAO_VAR, BLOCO, FIM},                             /* 25 */
    {SECAO_VAR, T_VAR, DECL_VAR, RESTO_VAR, FIM},               /* 26 */
    {SECAO_VAR, FIM},                                           /* 27 */
    {RESTO_VAR, DECL_VAR, RESTO_VAR, FIM},                      /* 28 */
    {RESTO_VAR, FIM},                                           /* 29 */
    {BLOCO, T_BEGIN, LISTA_CMD, T_END, FIM},                    /* 30 */
    {LISTA_CMD, CMD, RESTO_CMD, FIM},                           /* 31 */
    {RESTO_CMD, T_PONTO_VIRG, CMD, RESTO_CMD, FIM},             /* 32 */
    {RESTO_CMD, FIM},                                           /* 33 */
    {CMD, T_ID, RESTO_ID, FIM},                                 /* 34 */
    {CMD, T_IF, EXPR, T_THEN, CMD, SENAO, FIM},                 /* 35 */
    {CMD, T_WHILE, EXPR, T_DO, CMD, FIM},                       /* 36 */
    {CMD, BLOCO, FIM},                                          /* 37 */
    {CMD, FIM},                                                 /* 38 */
    {RESTO_ID, T_ATRIB, EXPR, FIM},                             /* 39 */
    {RESTO_ID, T_ABRE_PAR, LISTA_ARGS, T_FECHA_PAR, FIM},       /* 40 */
    {RESTO_ID, FIM},                                            /* 41 */
    {SENAO, T_ELSE, CMD, FIM},                                  /* 42 */
    {SENAO, FIM},                                               /* 43 */
    {LISTA_ARGS, EXPR, RESTO_ARGS, FIM},                        /* 44 */
    {RESTO_ARGS, T_VIRGULA, EXPR, RESTO_ARGS, FIM},             /* 45 */
    {RESTO_ARGS, FIM},                                          /* 46 */
    {EXPR, EXPR_SIMPLES, RESTO_EXPR, FIM},                      /* 47 */
    {RESTO_EXPR, T_OPREL, EXPR_SIMPLES, FIM},                   /* 48 */
    {RESTO_EXPR, FIM},                                          /* 49 */
    {EXPR_SIMPLES, TERMO, RESTO_SOMA, FIM},                     /* 50 */
    {RESTO_SOMA, T_OPSOMA, TERMO, RESTO_SOMA, FIM},             /* 51 */
    {RESTO_SOMA, T_OR, TERMO, RESTO_SOMA, FIM},                 /* 52 */
    {RESTO_SOMA, FIM},                                          /* 53 */
    {TERMO, FATOR, RESTO_MULT, FIM},                            /* 54 */
    {RESTO_MULT, T_OPMULT, FATOR, RESTO_MULT, FIM},             /* 55 */
    {RESTO_MULT, T_AND, FATOR, RESTO_MULT, FIM},                /* 56 */
    {RESTO_MULT, FIM},                                          /* 57 */
    {FATOR, T_ID, CHAMADA, FIM},                                /* 58 */
    {FATOR, T_NUM_INT, FIM},                                    /* 59 */
    {FATOR, T_NUM_REAL, FIM},                                   /* 60 */
    {FATOR, T_TRUE, FIM},                                       /* 61 */
    {FATOR, T_FALSE, FIM},                                      /* 62 */
    {FATOR, T_ABRE_PAR, EXPR, T_FECHA_PAR, FIM},                /* 63 */
    {FATOR, T_NOT, FATOR, FIM},                                 /* 64 */
    {FATOR, T_OPSOMA, FATOR, FIM},                              /* 65 */
    {CHAMADA, T_ABRE_PAR, LISTA_ARGS, T_FECHA_PAR, FIM},        /* 66 */
    {CHAMADA, FIM},                                             /* 67 */
    {PRINCIPAL, BLOCO, T_PONTO, FIM},                           /* 68 */
    {PRINCIPAL, FIM}                                            /* 69 */
};

int tamanho_producao(int p)
{
    int n = 0;
    while (prod[p][n + 1] != FIM)
        n++;
    return n;
}

void imprime_producao(int p)
{
    int i, n = tamanho_producao(p);
    printf("(%d) %s ->", p, nome_simbolo(prod[p][0]));
    if (n == 0)
        printf(" vazio");
    for (i = 1; i <= n; i++)
        printf(" %s", nome_simbolo(prod[p][i]));
}

/* ---------------- Tabela LL(1) ----------------
   tabela(A, t) devolve o numero da producao de A que deve ser usada
   quando o proximo token e t, ou 0 se nao tem producao (erro).
   As condicoes vem dos conjuntos FIRST e FOLLOW (ver relatorio). */

/* t pertence a FIRST(<expr>) ? */
int inicio_expr(int t)
{
    return t == T_ID || t == T_NUM_INT || t == T_NUM_REAL || t == T_TRUE ||
           t == T_FALSE || t == T_ABRE_PAR || t == T_NOT || t == T_OPSOMA;
}

/* t pertence a FOLLOW(<expr>) ? */
int fim_expr(int t)
{
    return t == T_THEN || t == T_DO || t == T_PONTO_VIRG || t == T_END ||
           t == T_ELSE || t == T_VIRGULA || t == T_FECHA_PAR;
}

/* t pertence a FOLLOW(<decl>) ? */
int fim_decl(int t)
{
    return t == T_VAR || t == T_ID || t == T_PROCEDURE || t == T_FUNCTION ||
           t == T_BEGIN || t == T_FIM;
}

int tabela(int A, int t)
{
    switch (A) {
    case PROGRAMA:
        if (t == T_PROGRAM || fim_decl(t)) return 1;
        break;
    case CABECALHO:
        if (t == T_PROGRAM) return 2;
        if (fim_decl(t)) return 3;
        break;
    case LISTA_DECL:
        if (t == T_VAR || t == T_ID || t == T_PROCEDURE || t == T_FUNCTION) return 4;
        if (t == T_BEGIN || t == T_FIM) return 5;
        break;
    case DECL:
        if (t == T_VAR) return 6;
        if (t == T_ID) return 7;
        if (t == T_PROCEDURE) return 8;
        if (t == T_FUNCTION) return 9;
        break;
    case DECL_VAR:
        if (t == T_ID) return 10;
        break;
    case LISTA_IDS:
        if (t == T_ID) return 11;
        break;
    case RESTO_IDS:
        if (t == T_VIRGULA) return 12;
        if (t == T_DOIS_PONTOS) return 13;
        break;
    case TIPO:
        if (t == T_INTEGER) return 14;
        if (t == T_REAL) return 15;
        if (t == T_BOOLEAN) return 16;
        break;
    case PROCEDIMENTO:
        if (t == T_PROCEDURE) return 17;
        break;
    case FUNCAO:
        if (t == T_FUNCTION) return 18;
        break;
    case PARAMS:
        if (t == T_ABRE_PAR) return 19;
        if (t == T_PONTO_VIRG || t == T_DOIS_PONTOS) return 20;
        break;
    case LISTA_PARAMS:
        if (t == T_ID) return 21;
        break;
    case RESTO_PARAMS:
        if (t == T_PONTO_VIRG) return 22;
        if (t == T_FECHA_PAR) return 23;
        break;
    case PARAM:
        if (t == T_ID) return 24;
        break;
    case CORPO:
        if (t == T_VAR || t == T_BEGIN) return 25;
        break;
    case SECAO_VAR:
        if (t == T_VAR) return 26;
        if (t == T_BEGIN) return 27;
        break;
    case RESTO_VAR:
        if (t == T_ID) return 28;
        if (t == T_BEGIN) return 29;
        break;
    case BLOCO:
        if (t == T_BEGIN) return 30;
        break;
    case LISTA_CMD:
        if (t == T_ID || t == T_IF || t == T_WHILE || t == T_BEGIN ||
            t == T_PONTO_VIRG || t == T_END) return 31;
        break;
    case RESTO_CMD:
        if (t == T_PONTO_VIRG) return 32;
        if (t == T_END) return 33;
        break;
    case CMD:
        if (t == T_ID) return 34;
        if (t == T_IF) return 35;
        if (t == T_WHILE) return 36;
        if (t == T_BEGIN) return 37;
        if (t == T_PONTO_VIRG || t == T_END || t == T_ELSE) return 38;
        break;
    case RESTO_ID:
        if (t == T_ATRIB) return 39;
        if (t == T_ABRE_PAR) return 40;
        if (t == T_PONTO_VIRG || t == T_END || t == T_ELSE) return 41;
        break;
    case SENAO:
        /* conflito do "else pendente": com else na entrada as duas producoes
           serviriam. Escolhemos a 42, que liga o else ao if mais proximo
           (mesma regra do Pascal). */
        if (t == T_ELSE) return 42;
        if (t == T_PONTO_VIRG || t == T_END) return 43;
        break;
    case LISTA_ARGS:
        if (inicio_expr(t)) return 44;
        break;
    case RESTO_ARGS:
        if (t == T_VIRGULA) return 45;
        if (t == T_FECHA_PAR) return 46;
        break;
    case EXPR:
        if (inicio_expr(t)) return 47;
        break;
    case RESTO_EXPR:
        if (t == T_OPREL) return 48;
        if (fim_expr(t)) return 49;
        break;
    case EXPR_SIMPLES:
        if (inicio_expr(t)) return 50;
        break;
    case RESTO_SOMA:
        if (t == T_OPSOMA) return 51;
        if (t == T_OR) return 52;
        if (fim_expr(t) || t == T_OPREL) return 53;
        break;
    case TERMO:
        if (inicio_expr(t)) return 54;
        break;
    case RESTO_MULT:
        if (t == T_OPMULT) return 55;
        if (t == T_AND) return 56;
        if (fim_expr(t) || t == T_OPREL || t == T_OPSOMA || t == T_OR) return 57;
        break;
    case FATOR:
        if (t == T_ID) return 58;
        if (t == T_NUM_INT) return 59;
        if (t == T_NUM_REAL) return 60;
        if (t == T_TRUE) return 61;
        if (t == T_FALSE) return 62;
        if (t == T_ABRE_PAR) return 63;
        if (t == T_NOT) return 64;
        if (t == T_OPSOMA) return 65;
        break;
    case CHAMADA:
        if (t == T_ABRE_PAR) return 66;
        if (fim_expr(t) || t == T_OPREL || t == T_OPSOMA || t == T_OR ||
            t == T_OPMULT || t == T_AND) return 67;
        break;
    case PRINCIPAL:
        if (t == T_BEGIN) return 68;
        if (t == T_FIM) return 69;
        break;
    }
    return 0;
}

/* ---------------- Tokens lidos pelo flex ---------------- */

#define MAX_TOKENS 100000

typedef struct {
    int tipo;
    char *lexema;
    int linha;
} Token;

Token tokens[MAX_TOKENS];
int num_tokens = 0;

/* producoes usadas, em ordem (formam a derivacao mais a esquerda) */
#define MAX_DERIV 400000
int derivacao[MAX_DERIV];
int num_deriv = 0;

/* ---------------- Pilha do automato ---------------- */

#define MAX_PILHA 100000   /* suporta milhares de niveis de aninhamento */

int pilha[MAX_PILHA];
int topo = 0;

void empilha(int s)
{
    if (topo >= MAX_PILHA) {
        printf("Erro: pilha cheia (limite de %d simbolos)\n", MAX_PILHA);
        exit(1);
    }
    pilha[topo++] = s;
}

void desempilha()
{
    topo--;
}

/* mostra a pilha com o topo a esquerda */
void imprime_pilha()
{
    int i;
    printf("  Pilha:   ");
    for (i = topo - 1; i >= 0; i--)
        printf("%s ", nome_simbolo(pilha[i]));
    printf("\n");
}

/* mostra os proximos tokens da entrada */
void imprime_entrada(int pos)
{
    int i;
    printf("  Entrada: ");
    for (i = pos; i < num_tokens && i < pos + 6; i++)
        printf("%s ", tokens[i].lexema);
    if (i < num_tokens)
        printf("...");
    printf("\n");
}

/* texto do token para as mensagens de erro */
char *texto_token(int pos)
{
    static char texto[300];
    if (tokens[pos].tipo == T_FIM)
        return "o fim do arquivo";
    sprintf(texto, "'%.200s'", tokens[pos].lexema);
    return texto;
}

/* lista os tokens que a tabela aceitaria para o nao-terminal A */
void imprime_esperados(int A)
{
    int t, primeiro = 1;
    for (t = 0; t < NUM_TERMINAIS; t++) {
        if (tabela(A, t) != 0) {
            printf(primeiro ? "%s" : ", %s", nome_terminal[t]);
            primeiro = 0;
        }
    }
}

int main(int argc, char **argv)
{
    int t, pos, passo, p, i, X, a;
    int detalhado = 1, aceito = 0;

    if (argc < 2) {
        printf("Uso: %s arquivo.pas [-r]\n", argv[0]);
        return 1;
    }
    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        printf("Nao foi possivel abrir o arquivo %s\n", argv[1]);
        return 1;
    }
    if (argc > 2 && strcmp(argv[2], "-r") == 0)
        detalhado = 0;

    printf("Reconhecedor miniPascal - arquivo: %s\n", argv[1]);

    /* ======== FASE 1: analise lexica ======== */
    if (detalhado) {
        printf("\n===== FASE 1: ANALISE LEXICA (flex) =====\n");
        printf("Linha  Token       Lexema\n");
    }
    do {
        t = yylex();
        if (t == T_ERRO) {
            printf("\nErro lexico na linha %d: caractere '%s' nao reconhecido\n", yylineno, yytext);
            printf("Programa REJEITADO.\n");
            return 1;
        }
        if (num_tokens >= MAX_TOKENS) {
            printf("Erro: arquivo grande demais\n");
            return 1;
        }
        tokens[num_tokens].tipo = t;
        tokens[num_tokens].lexema = strdup(t == T_FIM ? "$" : yytext);
        tokens[num_tokens].linha = yylineno;
        if (t == T_FIM && num_tokens > 0)   /* fim do arquivo: usa a linha do ultimo token */
            tokens[num_tokens].linha = tokens[num_tokens - 1].linha;
        if (detalhado)
            printf("%5d  %-10s  %s\n", yylineno, nome_terminal[t], tokens[num_tokens].lexema);
        num_tokens++;
    } while (t != T_FIM);

    /* ======== FASE 2: automato de pilha ======== */
    if (detalhado)
        printf("\n===== FASE 2: AUTOMATO DE PILHA (tabela LL(1)) =====\n");

    empilha(T_FIM);      /* fundo da pilha */
    empilha(PROGRAMA);   /* simbolo inicial */
    pos = 0;
    passo = 0;

    while (1) {
        X = pilha[topo - 1];      /* simbolo no topo da pilha */
        a = tokens[pos].tipo;     /* proximo token da entrada */
        passo++;

        if (detalhado) {
            printf("\nPasso %d\n", passo);
            imprime_pilha();
            imprime_entrada(pos);
        }

        if (X == T_FIM && a == T_FIM) {
            if (detalhado)
                printf("  Acao:    ACEITA (pilha so com $ e entrada toda lida)\n");
            aceito = 1;
            break;
        }

        if (eh_terminal(X)) {
            if (X == a) {
                /* terminal no topo igual ao token: desempilha e le o proximo */
                if (detalhado)
                    printf("  Acao:    le '%s' e desempilha %s\n", tokens[pos].lexema, nome_terminal[X]);
                desempilha();
                pos++;
            } else {
                printf("\nErro sintatico na linha %d: encontrado %s, esperado '%s'\n",
                       tokens[pos].linha, texto_token(pos), nome_terminal[X]);
                break;
            }
        } else {
            /* nao-terminal no topo: consulta a tabela */
            p = tabela(X, a);
            if (p == 0) {
                printf("\nErro sintatico na linha %d: encontrado %s ao reconhecer %s\n",
                       tokens[pos].linha, texto_token(pos), nome_simbolo(X));
                printf("Esperado um destes: ");
                imprime_esperados(X);
                printf("\n");
                break;
            }
            if (detalhado) {
                printf("  Acao:    ");
                imprime_producao(p);
                printf("\n");
            }
            if (num_deriv < MAX_DERIV)
                derivacao[num_deriv++] = p;
            /* troca A pelo lado direito, empilhando de tras para frente */
            desempilha();
            for (i = tamanho_producao(p); i >= 1; i--)
                empilha(prod[p][i]);
        }
    }

    /* ======== Resultado ======== */
    printf("\n===== RESULTADO =====\n");
    if (aceito) {
        printf("Programa ACEITO: o arquivo pertence a linguagem miniPascal.\n");
        printf("Tokens lidos: %d | Passos do automato: %d | Producoes aplicadas: %d\n",
               num_tokens - 1, passo, num_deriv);
        if (detalhado) {
            printf("Derivacao mais a esquerda (numeros das producoes):\n");
            for (i = 0; i < num_deriv; i++)
                printf("%d%s", derivacao[i], (i + 1) % 20 == 0 ? "\n" : " ");
            printf("\n");
        }
        return 0;
    }
    printf("Programa REJEITADO.\n");
    return 1;
}
