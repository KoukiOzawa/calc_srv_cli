#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdarg.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<stdarg.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include<pthread.h>

struct doCalc_t{
  double array[100];
  int sp;
  char user_input[100];
  char buf[100];
  int t_id;
};

typedef enum{
  TK_RESERVED, //記号
  TK_NUM, //整数トークン
  TK_EOF, //入力の終わりを表すトークン
}TokenKind;

typedef struct Token Token;

struct Token{
  TokenKind kind; //トークンの種類
  Token *next;
  int val; //数値
  char *str; //トークン文字列
  double len;  //トークンの長さ
  char eFlag;
};


typedef enum{
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM,
} NodeKind;

typedef struct Node Node;

//抽象構文木のノードの型
struct Node{
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  double val;
};

void gen(Node *node, struct doCalc_t *p);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize();
Node *expr();
extern char *user_input;
extern Token *token;

static void error_at(char *loc, char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  //* pos個分表示するという意味
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}


