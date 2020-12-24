#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<stdarg.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<errno.h>


typedef struct{
  unsigned int port;
  char *ip_address;
  char *msg;
  int sd;
  struct sockaddr_in sv_addr;
}cliet_info_t;

cliet_info_t *p_clinfo;

void error(char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr,fmt,ap);
  exit(1);
}

int server_connect(){
  int rc = -1;
  int count=0;
  //3回接続してみる
  while(rc != 0){
    rc = connect(p_clinfo->sd 
                ,(struct sockaddr *)&(p_clinfo->sv_addr)
                ,sizeof(p_clinfo->sv_addr));
    if(rc != 0){
      printf("errno:%d\n", errno);
      count++;
    }
    sleep(1);
    if(count == 3){
      rc = 16;
      error("接続失敗\n");
      exit(1);
    }
  }
  printf("connect complete\n");
  return rc;
}

#define MAX_MSG 100
int init(char *input[]){
  size_t size=0;
  int rc=0;

  //メッセージサイズ
  size += MAX_MSG;
  size += sizeof(cliet_info_t);
  p_clinfo = malloc(size);
  memset(p_clinfo, 0, size);
  p_clinfo->msg = malloc(MAX_MSG);
  if(p_clinfo->msg == NULL || p_clinfo == NULL){
    rc = 16; 
  }
  //ソケットの生成
  int sd = socket(AF_INET, SOCK_STREAM, 0);
  if(sd < 0){
    error("ソケット作成エラー\n");
  }else{
    p_clinfo->sd = sd;
  }


  //ip, port,etc
  p_clinfo->sv_addr.sin_family = AF_INET;
  p_clinfo->ip_address = input[1];
  p_clinfo->port = atoi(input[2]);
  p_clinfo->sv_addr.sin_addr.s_addr = inet_addr(p_clinfo->ip_address);
  p_clinfo->sv_addr.sin_port = htons(p_clinfo->port);

  return rc;
}

int do_query(){
  int rc=0;
  int len=0;
  char buf[BUFSIZ]="";

  while(1){
    memset(buf, 0, 100);
    printf("数式を入力してください\n");
    scanf("%s", buf);
    if(strncmp(buf, "#exit", 5) == 0)
      break;
    len = send(p_clinfo->sd, buf, strlen(buf), 0);
    if(len < 0) {
      printf("errno:%d", errno);
      error("send error\n");
      rc = 16;
      return rc;
    }
    printf("send:%s\n", buf);

    len = recv(p_clinfo->sd, buf, BUFSIZ, 0);
    if(len < 0) {
      error("recv error\n");
      rc = 16;
      return rc;
    }
    printf("ans:%s\n", buf);
  }


  return rc;
}

int main(int argc, char *argv[]){
  int rc;
  if(argc < 3){
    error("引数が足りません。address, port\n");
  }
  rc = init(argv);
  if(rc != 0){
    error("init error\n");
  }
  rc = server_connect();
  rc = do_query();
 return 0; 
}
