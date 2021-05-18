#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#define SIZE 1024
#define TCP_CONGESTION 13

void write_file(int sockfd)
{
  int num = 0;
  int n = 0;
  char *buffer = (char *)malloc(sizeof(char) * SIZE);

  while (num < (1024 * 1024))
  {
    n = recv(sockfd, buffer, SIZE, 0);
    if (n <= 0)
    {
      break;
    }
    num = num + n;
    bzero(buffer, SIZE);
  }
  printf("Size of bytes sent: %d\n", num);
  free(buffer);
  return;
}


int main()
{
  int port = 8080;
  int e;

  int sockfd, new_sock;
  struct sockaddr_in server_addr, new_addr;
  socklen_t addr_size;
  char buffer[SIZE];

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("There's an ERROR in the socket");
    exit(1);
  }
  printf(" Creating the server socket was successfully done \n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = INADDR_ANY;

  e = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (e < 0)
  {
    perror(" There's an ERROR in the bind ");
    exit(1);
  }
  printf(" Binding was successfully done \n");

  if (listen(sockfd, 10) == 0)
  {
    printf(" Listening right now... \n");
  }
  else
  {
    perror(" There's an ERROR in listening ");
    exit(1);
  }

  struct timeval before, after;
  addr_size = sizeof(new_addr);
  new_sock = accept(sockfd, (struct sockaddr *)&new_addr, &addr_size);

  //Writing and calculating the recieving time (in mili sec)
  gettimeofday(&before, NULL);
  for (int i = 0; i < 5; i++)
  {
    write_file(new_sock);
  }
  gettimeofday(&after, NULL);
  double avgCubic = (after.tv_sec - before.tv_sec) * 1000.0;
  avgCubic += (after.tv_usec - before.tv_usec) / 1000.0;
  avgCubic /= 5.0;
  printf(" The average time in cubic CC is: %lf ms\n", avgCubic);

  //Change CC
  char buf[SIZE];
  strcpy(buf, "reno");
  socklen_t len = strlen(buf);
  if (setsockopt(sockfd, IPPROTO_IP, TCP_CONGESTION, buf, len) != 0)
  {
    perror(" setsockopt ");
    return -1;
  }
  len = sizeof(buf);
  if (getsockopt(sockfd, IPPROTO_IP, TCP_CONGESTION, buf, &len) != 0)
  {
    perror(" getsockopt ");
    return -1;
  }
  gettimeofday(&before, NULL);
  for (int i = 0; i < 5; i++)
  {
    write_file(new_sock);
  }
  gettimeofday(&after, NULL);
  double avgReno = (after.tv_sec - before.tv_sec) * 1000.0;
  avgReno += (after.tv_usec - before.tv_usec) / 1000.0;
  avgReno /= 5.0;
  printf(" The average time in reno CC is: %lf ms\n", avgReno);
  close(new_sock);
  return 0;
}


// Reference:
// https://idiotdeveloper.com/file-transfer-using-tcp-socket-in-c/
// 