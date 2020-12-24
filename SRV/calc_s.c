#include"calc.h"
#include<errno.h>
#define LEN_QUERY 100
#define LEN_ANS 100

void error(char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr,fmt,ap);
  //exit(1);
}

struct server_info{
  unsigned short sv_port;
  int sd;//サーバディスクリプション
  int clnt_sd;//クライアントディスクリプション
  struct sockaddr_in sv_addr;
};
typedef struct server_info sv_info_t;
sv_info_t *p_svinfo_t;
struct sockaddr_in *cl_addr = NULL;

//スレッド間connect用mutex
pthread_mutex_t mp = PTHREAD_MUTEX_INITIALIZER;
//スレッド間処理用mutex
pthread_mutex_t th_mutex;
struct doCalc_t *p_doCalc;

void do_query(int sd, int id){
  int len=0;
  
  for(;;){
    memset(p_doCalc[id].buf, 0, LEN_QUERY);
    //memset(ans, 0, LEN_ANS);
    //文字列取得待機
    len = recv(sd, p_doCalc[id].user_input, 100, 0);
    if(len < 0){
      printf("errno:%d\n",errno);
     error("recv error\n");
    }
    printf("query:%s\n", p_doCalc[id].user_input);

    token = tokenize(&p_doCalc[id]); 
    if(token  == NULL){
      char buf[] = "トークナイズできません"; 
      len = send(sd, buf ,strlen(buf), 0);
      if(len < 0){
        printf("errno:%d\n",errno);
      }
      continue;
    }
    Node *node = expr();
    if(node == NULL){
      printf("can not gen\n");
      continue;
    }
    gen(node, &p_doCalc[id]);
    printf("gen ok\n");
    //calc
    double tmp = p_doCalc[id].array[0]; 
    sprintf(p_doCalc[id].buf ,"%.2f", tmp);
    puts(p_doCalc[id].buf);
    //respond ans
    len = send(sd, p_doCalc[id].buf, 100, 0);
  }
}

void server_connect(void *argv){
  int rc=0;
  int len=0;
  char buf[100]="okokok";
  struct sockaddr_in addr;
  int id; 
  id = *(int *)argv;
  
  printf("thread start %d\n",id);
  rc = pthread_detach(pthread_self());
  socklen_t clnt_len = (socklen_t)sizeof(struct sockaddr_in);
  //スレッドロック
  pthread_mutex_lock(&mp);
  int sd=0;

  while(1){
    //cl_addrにクライアントの情報を入れる
  //スレッド毎にacceptする
  printf("accept start:%d\n",p_svinfo_t->sd);
    sd = accept(p_svinfo_t->sd, (struct sockaddr*)&addr, &clnt_len);
    if(sd != -1){
      printf("sd:%d\n",sd);
      p_svinfo_t->clnt_sd = sd;
      printf("accept ok\n");
      break;
    }else{
      printf("sd:%d\n",sd);
   }
  }
  printf("connect ok\n");
  pthread_mutex_unlock(&mp);
  
  //process start
  do_query(sd, id);
}

int thread_create(){
  //クライアントの数だけスレッドを作成する
  //呼び出す関数はserver_connect関数
  pthread_t th_id;
  int soc;

  //初期化
  pthread_mutex_init(&mp, NULL);
  int thread_num[3]={1,2,3};
  printf("thread create start\n");
  for(int i=0;i < 3;i++){
    if(pthread_create(&th_id,NULL,(void*)server_connect,
          (void*)&thread_num[i])){
    
    }
    sleep(1);
  }

  return 0;
}


int server_init(char **argv){
  int rc=0;
  sv_info_t *info = malloc(sizeof(sv_info_t));
  if(info == NULL){
    error("server struct error\n");
  }
  memset(info, 0, sizeof(sv_info_t));

  info->sv_port = atoi(argv[1]);

  info->sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(info->sd < 0){
    error("ソケットが作成できません\n");
    exit(1);
  }

  info->sv_addr.sin_family=AF_INET;
  info->sv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  info->sv_addr.sin_port=htons(info->sv_port);

  rc = bind(info->sd, (struct sockaddr*)&(info->sv_addr),
      sizeof(info->sv_addr));
  if(rc != 0){
    error("bindができません\n");
    exit(1);
  }
  
  rc = listen(info->sd, 10);
  if(rc != 0){
    error("リッスンに失敗しました。\n");
  }
  printf("sd:%d\n",info->sd);

  //クライアント構造体
  p_svinfo_t = malloc(sizeof(sv_info_t));
  memset(p_svinfo_t, 0, sizeof(sv_info_t));

  //グローバル変数にサーバの情報を格納
  memcpy(p_svinfo_t, info, sizeof(sv_info_t));

  //計算用構造体をクライアントの数だけ作成
  p_doCalc = (struct doCalc_t*)malloc(sizeof(struct doCalc_t) * 3);
  if(p_doCalc == NULL){
    error("計算用構造体確保失敗\n");
    rc = 16;
    return rc;
  }
  return rc;
}
int main(int argc, char **argv){
  if(argc < 2){
    error("ポートを入力してください\n");
    return 0;
  }
  server_init(argv);
  printf("init ok\n");
  thread_create();
  printf("create ok\n");
  for(;;){
    sleep(10);
  }
  //server_connect();
  return 0;
}
