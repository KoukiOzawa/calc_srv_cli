#include"calc.h"

Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize();


Token *token;
char *user_input;

Token *new_token(TokenKind kind, Token *cur, char *str, int len){
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

Token *tokenize(struct doCalc_t *in){
  char *p = in->user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;
  printf("a:%s\n", p);

  //pには文字列、文字列の最後にはヌル文字が入っている
  while(*p){
    if(isspace(*p)){
      p++;
      continue;
    }

    //演算子トークン作成
    if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' ){
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if(strncmp(p, ">=", 2) == 0||  strncmp(p, "<=", 2) == 0|| strncmp(p, "==", 2) == 0|| strncmp(p, "!=", 2) == 0){
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

  if(*p == '<' || *p == '>'){
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
  }

    //数字トークン作成
    if(isdigit(*p)){
      printf("num\n");
      cur = new_token(TK_NUM, cur, p, 0);
      //strtolでポインタを進めている
      char *q = p;
      cur->val = (double)strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    //error_at(p, "トークナイズできません");
    return NULL;
    
  }
  new_token(TK_EOF, cur, p, 0);
  //最初のtokenはメモリ領域を確保していない
  //つまり中身空の目印みたいなのも
  return head.next;
}

