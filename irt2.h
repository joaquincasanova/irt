#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/i2c-dev.h>
#include <bcm2835.h>
#include <time.h>
#include <fstream>

using namespace std;
 
#define MLX_I2CADDR       0x5a 
#define MLX_TREG          0x07
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
double tempfactorMLX = 0.02;
double tempfactorTMP = 0.03125;

class MLX{
	char address;
	char reg;
	char buf[6];
	double temp;
	public:
		MLX(char);
		int read();
}

MLX::MLX(char SA){
	address = SA;
	bcm2835_i2c_setSlaveAddress(address); 
}

int MLX::read() {
	reg=MLXTREG;
	bcm2835_i2c_read_register_rs(&reg, &buf[0],3);
	temp = tempfactorMLX * (double)(buf[0]+(buf[1]<<8));
   
	return 0;
}

class TMP{
	char address;
	char reg;
	char buf[6];
	double temp;
	public:
		TMP(char);
		int read();		
}

TMP::TMP(char SA){
	address = SA;
	bcm2835_i2c_setSlaveAddress(address); 
	reg = TMP007_CONFIG;
	buf[0] = reg;
	buf[1] = ((TMP007_CFG_MODEON | TMP007_CFG_ALERTEN |  TMP007_CFG_TRANSC | TMP007_CFG_16SAMPLE)>>8);
	buf[2] = ((TMP007_CFG_MODEON | TMP007_CFG_ALERTEN |  TMP007_CFG_TRANSC | TMP007_CFG_16SAMPLE));
	bcm2835_i2c_write(&buf[0],3);

	reg = TMP007_STATMASK;
	buf[0] = reg;
	buf[1] = ((TMP007_STAT_ALERTEN |TMP007_STAT_CRTEN)>>8);
	buf[2] = ((TMP007_STAT_ALERTEN |TMP007_STAT_CRTEN));
	bcm2835_i2c_write(&buf[0],3);
}

int TMP::read() {
  reg = TMP007_TOBJ;
  bcm2835_i2c_read_register_rs(&reg, &buf[0],3);
  temp = tempfactorTMP * (double)(((buf[0]<<8)+buf[1])>>2);
   
  return 0;
}

int main(){

  if(!bcm2835_init()){
    cout<<"Failed bcm2835 init"<<std::endl;
    return -1;
  };

  bcm2835_i2c_begin();
  bcm2835_i2c_set_baudrate(25000);
  
  MLX MLX1(0x5A);
    
  MLX.read();
  
  cout << MLX.temp << std::endl;
  
  TMP TMP1(0x40);
    
  TMP.read();
  
  cout << TMP.temp << std::endl;
  
  bcm2835_i2c_end();
  bcm2835_close();

  return 0;

}
