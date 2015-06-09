//client side socket functions
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "diffie_hellman.h"

#define BUFFSIZE 1024

struct s_detail{
  int connfd;
  char name[64];
  int port;
};

//write  to file
int gwrite_to_file(char* data, char *fname){
  FILE* fp;
  fp = fopen(fname,"w");
  if(!fp){
    printf("oops");
    fprintf(stderr,"Cannot open file\n");
    return 0;
  }

  fprintf(fp,"%s",data);
  fclose(fp);
  return 1;
}

//send  to server
int c_gsend(char *fname){
  int sockfd=0;
  char sendBuff[BUFFSIZE];
  struct sockaddr_in serv_addr;
  char buff[BUFFSIZE];

  char send_ip[32]="127.0.0.1";

  memset(sendBuff, '0', sizeof(sendBuff));
  memset(&serv_addr, '0', sizeof(serv_addr));

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0){
    printf("\n Error: Could not create socket \n");
    return 0;
  }
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(5000);

  if(inet_pton(AF_INET, send_ip, &serv_addr.sin_addr)<=0){
    printf("\n inet_pton error occured \n");
    return 0;
  }

  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
    printf("\n Error: Connect failed \n");
    return 0;
  }

  FILE *fp;
  fp = fopen(fname,"r");
  if(!fp){
    printf("Error opening file\n");
    return 0;
  }

  while(1){
    char buff[BUFFSIZE]={0};
    int nread = fread(buff,1,BUFFSIZE,fp);
    printf("Bytes read %d\n",nread);

    if(nread > 0){
      printf("Sending....\n");
      write(sockfd, buff, nread);
    }

    if(nread < BUFFSIZE){
      if(feof(fp))
        printf("End of file\n");
      if(ferror(fp))
        printf("error reading\n");
      break;
    }
  }
  
  puts("graph sent to server");
  close(sockfd);
  return 1;
}


//receive  from server
int c_grecv(){
  int listenfd = 0, connfd=0;
  struct sockaddr_in serv_addr, client_addr;
  int bytesReceived=0;
  char recvBuff[1024];

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&client_addr, '0', sizeof(client_addr));
  memset(&serv_addr, '0', sizeof(serv_addr));
  memset(recvBuff, '0', sizeof(recvBuff));
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  serv_addr.sin_port = htons(5001);

  bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
  
  if(listen(listenfd, 5) == -1){
    printf("Failed to listen. Max limit reached.\n");
    return 1;
  }

  int addrlen = sizeof(struct sockaddr_in);
  //struct s_detail sh_args;
  
  puts("Client is ready and waiting...");

  connfd = accept(listenfd, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);
  printf("connected. server ip: %s and port: %d\n",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
/*
  sh_args.connfd = connfd;
  sh_args.port = ntohs(client_addr.sin_port);
  strcpy(sh_args.name,inet_ntoa(client_addr.sin_addr));
  
  printf("writing to %s",args->name);
  */

  while((bytesReceived = read(connfd, recvBuff, sizeof(recvBuff)-1)) > 0){
      puts("receiving.....");
      recvBuff[bytesReceived] = '\0';
      FILE *fp;
      fp = fopen("recd_bobkey","w");
      fwrite(recvBuff, 1, bytesReceived, fp);
      fclose(fp);
    }

  if(bytesReceived < 0)
    printf("\n Read error \n");

  puts("Key received from server");
  close(connfd);
  return 0;
  
}

void gen_write_key(BIGNUM* key, BIGNUM* a, char* fname){
    BIGNUM *p, *g;
    p = BN_new();
    g = BN_new();
    BN_hex2bn(&p, df_p_hex1024_str); 
    BN_hex2bn(&g, df_g_hex1024_str);
    get_random_prime(1024, a, 0);
    mod_exp_bin(key, g, a, p);  // g^a (mod p)

    char* key_str = BN_bn2dec(key);
    gwrite_to_file( key_str, fname);

    free(key_str);
    BN_clear_free(p);
    BN_clear_free(g);
}

void gen_common_key(BIGNUM *result, BIGNUM *a, char* fname){
    BIGNUM *p, *key;
    p = BN_new();
    key = BN_new();
    BN_hex2bn(&p, df_p_hex1024_str); 
    
  FILE *fp;
  fp = fopen(fname,"r");
  if(!fp){
    printf("Error opening file\n");
    return 0;
  }

    char buff[BUFFSIZE]={0};
    int nread = fread(buff,1,BUFFSIZE,fp);
    fclose(fp);

    BN_dec2bn(&key, buff);
    mod_exp_bin(result, key, a, p);  // key^b (mod p) = g^ab

    BN_clear_free(p);
    BN_clear_free(key);
}

int main(int argc, char *argv[]){
    BIGNUM * a = BN_new();
    BIGNUM * key = BN_new();
    BIGNUM * result = BN_new();
  char *fname;
  fname="alice_key";

  gen_write_key(key, a, fname);
  puts("reading key from file...\n");

  //send key to server
  puts("sending key to server...");
  if(!c_gsend(fname)){
    printf("sending failed for %s",fname);
  }
  puts("waiting to receive key from server...");
  c_grecv();
  
  puts("*************************");
  
  gen_common_key(result, a, "recd_bobkey");
  printf("Final key at Alice:\n");
  print_bn(result);

  BN_clear_free(a);
  BN_clear_free(key);
  BN_clear_free(result);
  return 0;
}
