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

#define BUFLEN 1400
#define PORT 5005
#define SRV_IP "192.168.1.2"

int main(int argc, char **argv)
{
  struct timeval tv1, tv2;
  pruIo *io = pruio_new(PRUIO_DEF_ACTIVE, 0x98, 0, 1); //! create new driver structure
  if (io->Errr) {
    printf("initialisation failed (%s)\n", io->Errr);
    return 0;
  }

  if (pruio_config(io, 1, 0x1FE, 0, 4)) {
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
  int j = 0;
  int i = 0;
  int numreps = 1000;
  gettimeofday(&tv1, NULL);
  for (i = 0; i < numreps; i++){
      j = 0;
      while ( j < BUFLEN ){
          //gettimeofday(&tv, NULL);
          //t = 1000000 * tv.tv_sec + tv.tv_usec
          //    - 1000000 * tv1.tv_sec - tv1.tv_usec
          buf[j + 4] = io->Adc->Value[0];
          memcpy(&buf[j], &t, sizeof(uint32_t));
          j = j + 5;
      }
      sendto(s, buf, BUFLEN, MSG_DONTWAIT | MSG_NOSIGNAL,
          (struct sockaddr *)&si_other, slen);
  }
  gettimeofday(&tv2, NULL);
  close(s);

  uint32_t t1 = 1000000 * tv1.tv_sec + tv1.tv_usec;
  uint32_t t2 = 1000000 * tv2.tv_sec + tv2.tv_usec;
  int numsamples = BUFLEN / 5;
  printf("rate: %f kSps\n", numsamples * numreps * 1.0E3
      / static_cast<float>(t2 - t1));
  pruio_destroy(io);
	return 0;
}
