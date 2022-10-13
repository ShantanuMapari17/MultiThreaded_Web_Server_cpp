/* run using ./server <port> */
#include<bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "http_server.hh"
#include <netinet/in.h>

#include <pthread.h>
pthread_cond_t fill_buf;
pthread_mutex_t buf_lock;


queue<int> shared_queue;

void error(char *msg) {
  perror(msg);
  exit(1);
}

void * startFun(void *args){
  // int newsockfd=*((int *)args);

    while (1)
    {
        pthread_mutex_lock(&buf_lock);
      while(shared_queue.empty())
        pthread_cond_wait(&fill_buf,&buf_lock);
      int newsockfd=shared_queue.front();
      shared_queue.pop();
      pthread_mutex_unlock(&buf_lock);

      char buffer[1024];
      int n;

    
      bzero(buffer, 1024);
      n = read(newsockfd, buffer, 1023);
      if(n==0){
        close(newsockfd);
        // return;
      }
      else if (n < 0){
        error("ERROR reading from socket");
      }
      else{
      printf("Here is the message: %s", buffer);
      
      /* send reply to client */
      HTTP_Request *request = new HTTP_Request(buffer);
      HTTP_Response *response = handle_request(string(buffer));
      string buff2= response->get_string();


      n = write(newsockfd,buff2.c_str(),buff2.length());
      if (n < 0)
        error("ERROR writing to socket");

      close(newsockfd); 
      }
    }
    

}


int main(int argc, char *argv[]) {
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  pthread_t tid[10];

  pthread_mutex_init(&buf_lock,NULL);
  pthread_cond_init(&fill_buf,NULL);

  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  /* create socket */

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  /* fill in port number to listen on. IP address can be anything (INADDR_ANY)
   */

  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* bind socket to this port number on this machine */

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");

  /* listen for incoming connection requests */

  for(int i=0;i<10;i++){
    pthread_create(&tid[i],NULL,startFun,NULL);
  }

  listen(sockfd, 5);
  while (1)
  {
      /* listen for incoming connection requests */
      clilen = sizeof(cli_addr);

      /* accept a new request, create a newsockfd */
      newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
      if (newsockfd < 0)
        error("ERROR on accept");

      pthread_mutex_lock(&buf_lock);
      shared_queue.push(newsockfd);
      pthread_cond_signal(&fill_buf);
      pthread_mutex_unlock(&buf_lock);
      
      // pthread_create(&tid,NULL,startFun,&newsockfd);    
    
  }
  

  
  // for(int i=0;i<10;i++){
  //   pthread_join(tid[i],NULL);
  // }

  close(sockfd);
  close(newsockfd);

  return 0;
}
