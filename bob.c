//used by server to receive data from clients
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>

#include "diffie_hellman.h"

#define NUM_OF_CLIENTS 1
#define BUFFSIZE 1024

struct c_detail{
  int connfd;
  char name[256];
  int port;
};

struct c_detail ch_args[NUM_OF_CLIENTS];

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

void *connection_handler(void *ch_args){
  int bytesReceived;
  char recvBuff[1024];
  struct c_detail *args = ch_args;
  //int sock = *(int*)sock_desc;

  //printf("In connection handler for client: %s\n",args->name);

  char port[32];
  sprintf(port,"%d",args->port);
  //strcat(args->name,"$");
  //strcat(args->name,port);
 // printf("writing to file %s\n",args->name);
  fflush(stdout);
  while((bytesReceived = read(args->connfd, recvBuff, sizeof(recvBuff)-1)) > 0){
      recvBuff[bytesReceived] = '\0';
      FILE *fp;
      //fp = fopen(args->name,"w");
      fp = fopen("recd_alicekey","w");
      //puts("receiving.....");
      //printf("data:%s\n", recvBuff);
      fwrite(recvBuff, 1, bytesReceived, fp);
      fclose(fp);
    }

  //puts("im here");
  //write(args->connfd,"I got your message",18);

  if(bytesReceived < 0)
    printf("\n Read error \n");

  close(args->connfd);
  return 0;
}

int s_grecv(const char *dirname){
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
  serv_addr.sin_port = htons(5000);

  bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
  
  if(listen(listenfd, 5) == -1){
    printf("Failed to listen. Max limit reached.\n");
    return 1;
  }
  
  int addrlen = sizeof(struct sockaddr_in);
  int c_num=0;
  pthread_t thread_id;
  puts("I'm ready for client");

  while(c_num!=NUM_OF_CLIENTS){
    connfd = accept(listenfd, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);
    printf("connected client %d. client ip: %s and port: %d\n",c_num, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    c_num++; 
    //printf("a\n");
    ch_args[c_num].connfd = connfd;
    //printf("a\n");
    ch_args[c_num].port = ntohs(client_addr.sin_port);
    //printf("a\n");
    //strcat(ch_args[c_num].name,dirname);
    //printf("a\n");
    //strcat(ch_args[c_num].name,inet_ntoa(client_addr.sin_addr));
    //printf("fpath!!!!! :::: :::: %s\n",ch_args[c_num].name);
    fflush(stdout);
    if ( pthread_create(&thread_id,NULL, connection_handler, &ch_args[c_num]) <0 ){
      perror("unable to create thread");
      return 1;
    }
    //puts("handler assigned");
    //printf("%d",thread_id);
    pthread_join(thread_id,NULL);  
  }
//sleep not required
  sleep(5);
  puts("just woke up. hope all threads are done.");
  //pthread_exit(NULL);
  close(connfd);

  return 0;
}

//send  to all clients
int s_gsend(char *fname){
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

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(5001);

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
  
  puts("key sent to client");
  close(sockfd);
  return 1;
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
  fname="bob_key";

  gen_write_key(key, a, fname);
  //const char *dirname = "./key/";
  const char *dirname = "recd_alicekey";
  s_grecv(dirname);

  //printf("bc file is in: %s\n",fname);
  //puts("-------------------------");
  puts("I will send the key now");
  if(!s_gsend(fname)){
    puts("We have Problems!!");
  }
  puts("*************************");
  
  gen_common_key(result, a, "recd_alicekey" );
  printf("Final key at Bob:\n");
  print_bn(result);
  BN_clear_free(a);

  BN_clear_free(key);
  BN_clear_free(result);
  return 0;
}
