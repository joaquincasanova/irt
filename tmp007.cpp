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

short int reverse_byte_order(short int msg){
  return ((msg<<8) & 0xFF00)|((msg>>8) & 0x00FF);
}

class TMP{
private:
  int fd;
  double temp;
  short unsigned int msg;
public:
  double objTemp();
  double dieTemp();
  TMP(void);
  ~TMP(void);
};

TMP::TMP(void){
  cout << "Opening I2C" << std::endl;
  if ((fd = open(filename, O_RDWR)) < 0) {        // Open port for reading and writing
    perror("Failed to open i2c port");
    //return -1;
  }

  cout << "TMP I2C" << std::endl;       
  if (ioctl(fd, I2C_SLAVE, TMP007_I2CADDR) < 0) {        // Set the port options and set the address of the device we wish to speak to
    perror("Unable to get bus access to talk to slave");
    close(fd);
    //return -1;
  }
  
  cout << "TMP WRITE I2C CONFIG" << std::endl;
  msg = TMP007_CFG_MODEON | TMP007_CFG_ALERTEN |  TMP007_CFG_TRANSC | TMP007_CFG_16SAMPLE;
  msg = reverse_byte_order(msg);
  i2c_smbus_write_word_data(fd, TMP007_CONFIG, msg);

  cout << "TMP WRITE I2C STATUS" << std::endl;
  msg = TMP007_STAT_ALERTEN |TMP007_STAT_CRTEN;
  msg = reverse_byte_order(msg);     
  i2c_smbus_write_word_data(fd, TMP007_STATMASK, msg);
}

double TMP::objTemp(void){
  cout << "TMP READ I2C OBJ TEMP" << std::endl;       
  msg = i2c_smbus_read_word_data(fd, TMP007_TOBJ);
  msg = reverse_byte_order(msg); 
  temp = tempfactorTMP*double(msg>>2);
  cout << temp << " C" << std::endl;
  return temp;
}

double TMP::dieTemp(void){
  cout << "TMP READ I2C DIE TEMP" << std::endl;       
  msg = i2c_smbus_read_word_data(fd, TMP007_TDIE);
  msg = reverse_byte_order(msg); 
  temp = tempfactorTMP*double(msg>>2);
  cout << temp << " C" << std::endl;
  return temp;
}

TMP::~TMP(void){
  //return close(fd);
}

int main(void){

  TMP test;
  
  test.objTemp();
  test.dieTemp();
  return 0;

};
