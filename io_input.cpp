#define _GNU_SOURCE 1
#include <stdint.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include "/root/libpruio-0.2/src/c_wrapper/pruio.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define BUFLEN 1406
#define PORT 5005
#define SRV_IP "192.168.1.2"
#define PRUIO_DEF_AVERAGE 1

int main(int argc, char **argv)
{
  struct timeval tv1, tv2, tv;
  struct sockaddr_in si_other;
  int s, j, slen = sizeof(si_other);
  char buf[BUFLEN];
  uint32_t t;
  
  
  pruIo *io = pruio_new(PRUIO_ACT_ADC, 0, 0, 0); //! create new driver structure
  if (io->Errr) {
    printf("initialisation failed (%s)\n", io->Errr);
    return 0;
  }

  if (pruio_config(io, 1, 0x1FE, 500, 4)) {
    printf("config failed (%s)\n", io->Errr);
    return 0;
  }

  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) ==- 1)
      perror("socket");

  memset((char *)&si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
  if (inet_aton(SRV_IP, &si_other.sin_addr) == 0) {
      perror("inet_aton() failed");
  }


  uint32_t t1;
  uint32_t val;
  int i = 0;
  int numreps = 100;
  usleep(1000);
  gettimeofday(&tv1, NULL);
  t1 = 1000000 * tv1.tv_sec + tv1.tv_usec;
  t = 0;
  for (i = 0; i < numreps; i++)
  {
      j = 0;
      while ( j < BUFLEN ){
          gettimeofday(&tv, NULL);
	  val = io->Adc->Value[1] >> 4;
	  memcpy(&buf[j], &val, sizeof(val));
          t = 1000000 * tv.tv_sec + tv.tv_usec;
          memcpy(&buf[j+2], &t, sizeof(uint32_t));
          j = j + 6;
   }
      sendto(s, buf, BUFLEN, MSG_DONTWAIT | MSG_NOSIGNAL,
          (struct sockaddr *)&si_other, slen);
  }
  gettimeofday(&tv2, NULL);
  close(s);

  t1 = 1000000 * tv1.tv_sec + tv1.tv_usec;
  uint32_t t2 = 1000000 * tv2.tv_sec + tv2.tv_usec;
  int numsamples = BUFLEN / 5;
  printf("rate: %f kSps\n", numsamples * numreps * 1.0E3
      / static_cast<float>(t2 - t1));
  pruio_destroy(io);
	return 0;
}
