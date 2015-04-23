#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/i2c-dev.h>

using namespace std;
 
#define TMP007_VOBJ       0x00
#define TMP007_TDIE       0x01
#define TMP007_CONFIG     0x02
#define TMP007_TOBJ       0x03
#define TMP007_STATUS     0x04
#define TMP007_STATMASK   0x05

#define TMP007_CFG_RESET    0x8000
#define TMP007_CFG_MODEON   0x1000
#define TMP007_CFG_1SAMPLE  0x0000
#define TMP007_CFG_2SAMPLE  0x0200
#define TMP007_CFG_4SAMPLE  0x0400
#define TMP007_CFG_8SAMPLE  0x0600
#define TMP007_CFG_16SAMPLE 0x0800
#define TMP007_CFG_ALERTEN  0x0100
#define TMP007_CFG_ALERTF   0x0080
#define TMP007_CFG_TRANSC   0x0040

#define TMP007_STAT_ALERTEN 0x8000
#define TMP007_STAT_CRTEN   0x4000

#define TMP007_I2CADDR 0x40
#define TMP007_DEVID 0x1F

const char *filename = "/dev/i2c-1";
double tempfactorTMP = 0.03125;

int main(void){

  int fd;
  double temp;

  cout << "Opening I2C" << std::endl;
  if ((fd = open(filename, O_RDWR)) < 0) {        // Open port for reading and writing
    perror("Failed to open i2c port");
    return -1;
  }

  cout << "TMP I2C" << std::endl;       
  if (ioctl(fd, I2C_SLAVE, TMP007_I2CADDR) < 0) {        // Set the port options and set the address of the device we wish to speak to
    perror("Unable to get bus access to talk to slave");
    close(fd);
    return -1;
  }
  
  cout << "TMP WRITE I2C" << std::endl;       
  i2c_smbus_write_word_data(fd, TMP007_CONFIG, TMP007_CFG_MODEON | TMP007_CFG_ALERTEN |  TMP007_CFG_TRANSC | TMP007_CFG_16SAMPLE);

  cout << "TMP WRITE I2C" << std::endl;       
  i2c_smbus_write_word_data(fd, TMP007_STATMASK, TMP007_STAT_ALERTEN |TMP007_STAT_CRTEN);

  cout << "TMP READ I2C" << std::endl;       
  temp=tempfactorTMP*double(i2c_smbus_read_word_data(fd, TMP007_TOBJ)>>2);
  cout << temp << " C" << std::endl;
  close(fd);
  
  return 0;

};
