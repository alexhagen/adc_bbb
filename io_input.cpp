//! Message for the compiler.
#define _GNU_SOURCE 1
#include <stdint.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include "/root/libpruio-0.2/src/c_wrapper/pruio.h"
#include <sys/time.h>

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

  int a;
  int numreps = 1000;
  gettimeofday(&tv1, NULL);
  for (int i = 0; i < numreps; i++){
    a = io->Adc->Value[0];
  }
  gettimeofday(&tv2, NULL);

  uint32_t t1 = 1000000 * tv1.tv_sec + tv1.tv_usec;
  uint32_t t2 = 1000000 * tv2.tv_sec + tv2.tv_usec;
  printf("rate: %f kSps\n", numreps * 1.0E3
      / static_cast<float>(t2 - t1));

  /*
  struct termios oldt, newt; //             make terminal non-blocking
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  newt.c_cc[VMIN] = 0;
  newt.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  */
  /*
  printf("\r%8X %8X %8X %8X  %4d %4X %4X %4X %4X %4X %4X %4X"
       , io->Gpio->Raw[0]->Mix, io->Gpio->Raw[1]->Mix, io->Gpio->Raw[2]->Mix, io->Gpio->Raw[3]->Mix
       , io->Adc->Value[1], io->Adc->Value[2], io->Adc->Value[3], io->Adc->Value[4]
       , io->Adc->Value[5], io->Adc->Value[6], io->Adc->Value[7], io->Adc->Value[8]);

  //tcsetattr(STDIN_FILENO, TCSANOW, &oldt); //           reset terminal
  */
  pruio_destroy(io);       /* destroy driver structure */
	return 0;
}
