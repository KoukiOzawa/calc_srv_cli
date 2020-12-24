#include"calc.h"

Node *primary();
Node *add();
Node *relational();
Node *equality();
Node *mul();
Node *unary();
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(double val);

//期待している記号の時には、トークンを1つ読み進めて
//真を返す。
bool consume(char *op){
  if(token->kind != TK_RESERVED || 
    token->len != strlen(op) || 
    memcmp(token->str, op, token->len))
      return false;
  token = token->next;
  return true;
}

//トークンを一つ進める
void expect(char *op){
  if(token->kind != TK_RESERVED ||
      token->len != strlen(op) || 
      memcmp(token->str, op, token->len))
      error_at(token->str, "'%s'ではありません", op);
  token = token->next;
}

//トークンを一つ進めて、数字を返す
double expect_number(char *p){
  if(token->kind != TK_NUM){
    *p = 1;
    printf("it is not number\n");
    return 0;
  }
  double val = token->val;
  token = token->next;
  return val;
}

bool at_eof(){
  return token->kind == TK_EOF;
}


//2項演算子
Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

//数値用
Node *new_node_num(double val){
  Node *node = calloc(1,sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

//expr = equality 
Node *expr(){
  //Node *node = equality();
  Node *node = add();
  return node;

}

//add = mul("+" mul | "-" mul)*
Node *add(){
  Node *node = mul();
  
  for(;;){
    if(consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if(consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

//単項演算子を追加unary
//mul = unary("*" unary | "/" unary)*
Node *mul(){
  Node *node = unary();

  for(;;){
    if(consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if(consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

//unary = ("+"  | "-" )? primary
Node *unary(){
  if(consume("+"))
    return primary();
  if(consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

//primary = num | "(" expr ")"
Node *primary(){
  if(consume("(")){
    Node *node = expr();
    expect(")");
    return node;
  }

  //今の所一番最初のexpr()でここに来る
  char flag= '0';
  double num = expect_number(&flag);
  if(flag == '1')
    return NULL;
  return new_node_num(num);
}

void push(double num, struct doCalc_t *p){
  int sp = p->sp;
  p->array[sp] = num;
  p->sp++;
}

double pop(struct doCalc_t *p){
  p->sp--;
  int sp = p->sp;
  return p->array[sp];
}

void gen(Node *node, struct doCalc_t *p){
  if(node->kind == ND_NUM){
    printf(" push %f\n", node->val);
    push(node->val, p);
    return;
  }

  gen(node->lhs, p);
  gen(node->rhs, p);

  //pushした数字をrdi, raxにpopする
  printf(" pop \n");
  printf(" pop \n");

  double a,b;
  a = pop(p);
  b = pop(p);
  double sum=0;
  switch(node->kind){
  case ND_NUM:
    printf("来るんかい\n");
    //このパスには来ないはず
    break;
  case ND_ADD:
    printf(" add\n");
    sum = a + b;
    break;
  case ND_SUB:
    printf(" sub\n");
    sum = a - b;
    break;
  case ND_MUL:
    printf(" mul\n");
    sum = a * b;
    break;
  case ND_DIV:
    printf(" div\n");
    sum = a / b;
    break;
  /*  
  case ND_EQU:
    printf(" cmp rax, rdi\n");
    printf(" sete al\n");
    printf(" movzb rax, al\n");
    break;
  case ND_NEQ:
    printf(" cmp rax, rdi\n");
    printf(" setne al\n");
    printf(" movzb rax, al\n");
    break;
  case ND_SMA://"<"
    printf(" cmp rax, rdi\n");
    printf(" setl al\n");
    printf(" movzb rax, al\n");
    break;
  case ND_ESM://"<="
    printf(" cmp rax, rdi\n");
    printf(" setel al\n");
    printf(" movzb rax, al\n");
    break;
  case ND_GRE://"a > b" -> "b <= a"
    printf(" cmp rdi, rax\n");
    printf(" setel al\n");
    printf(" movzb rax, al\n");
    break;
  case ND_EGR://"a >= b" -> "b < a"
    printf(" cmp rdi, rax\n");
    printf(" setl al\n");
    printf(" movzb rax, al\n");
    break;
    */
  }

  //演算子で計算したものをpushする
  printf(" push sum\n");
  push(sum, p);
}

