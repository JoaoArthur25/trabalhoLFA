/* tokens.h - codigos dos tokens (terminais) do miniPascal
   O flex (pascal.l) devolve esses valores para o main.c */

#ifndef TOKENS_H
#define TOKENS_H

#define T_FIM          0   /* fim do arquivo, o $ da gramatica */
#define T_PROGRAM      1
#define T_VAR          2
#define T_PROCEDURE    3
#define T_FUNCTION     4
#define T_BEGIN        5
#define T_END          6
#define T_IF           7
#define T_THEN         8
#define T_ELSE         9
#define T_WHILE       10
#define T_DO          11
#define T_INTEGER     12   /* tipo integer */
#define T_REAL        13   /* tipo real */
#define T_BOOLEAN     14   /* tipo boolean */
#define T_AND         15
#define T_OR          16
#define T_NOT         17
#define T_TRUE        18
#define T_FALSE       19
#define T_ID          20   /* identificador */
#define T_NUM_INT     21   /* numero inteiro: 10 */
#define T_NUM_REAL    22   /* numero real: 0.75 */
#define T_ATRIB       23   /* := */
#define T_OPREL       24   /* =  <>  <  <=  >  >= */
#define T_OPSOMA      25   /* +  - */
#define T_OPMULT      26   /* *  /  div  mod */
#define T_PONTO_VIRG  27   /* ; */
#define T_DOIS_PONTOS 28   /* : */
#define T_VIRGULA     29   /* , */
#define T_ABRE_PAR    30   /* ( */
#define T_FECHA_PAR   31   /* ) */
#define T_PONTO       32   /* . */

#define NUM_TERMINAIS 33

#define T_ERRO        99   /* caractere que nao pertence a linguagem */

#endif
