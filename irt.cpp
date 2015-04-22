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
 
#define MLX_I2CADDR = 0x5a 
#define MLX_TREG = 0x07
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

int MLX90614_read(int smbusfd, double temp) {
	union i2c_smbus_data msg;
	struct i2c_smbus_ioctl_data sdat = {
		sdat.read_write = I2C_SMBUS_READ,
		sdat.command = MLX_TREG,  // read register 7, ir
		sdat.size = I2C_SMBUS_WORD_DATA,
		sdat.data = &msg
	};
	int res;
	if ((res = ioctl(smbusfd, I2C_SMBUS, &sdat)) < 0) {
	  return res;  // maybe not the best value
	}
	temp = tempfactorMLX * (double)msg.word;
	
	return 0;
}

int TMP007_read(int smbusfd, double temp) {
	union i2c_smbus_data msg;
	struct i2c_smbus_ioctl_data sdat = {
		sdat.read_write = I2C_SMBUS_READ,
		sdat.command = TMP007_TOBJ,  // read register 3, ir
		sdat.size = I2C_SMBUS_WORD_DATA,
		sdat.data = &msg
	};
	int res;
	if ((res = ioctl(smbusfd, I2C_SMBUS, &sdat)) < 0) {
	  return res;  // maybe not the best value
	}
	temp = tempfactorTMP * double(msg.word>>2);
	
	return 0;
}

int TMP007_write(int smbusfd) {

	union i2c_smbus_data msg;
	msg.word = TMP007_CFG_MODEON | TMP007_CFG_ALERTEN |  TMP007_CFG_TRANSC | TMP007_CFG_16SAMPLE;
	struct i2c_smbus_ioctl_data sdat = {
		sdat.read_write = I2C_SMBUS_WRITE,
		sdat.command = TMP007_CONFIG,  // Write to config register
		sdat.size = I2C_SMBUS_WORD_DATA,
		sdat.data = &msg
	};
	int res;
	if ((res = ioctl(smbusfd, I2C_SMBUS, &sdat)) < 0) {
	  return res;  // maybe not the best value
	}
	
	msg.word = TMP007_STAT_ALERTEN |TMP007_STAT_CRTEN;
	struct i2c_smbus_ioctl_data sdat = {
		sdat.read_write = I2C_SMBUS_WRITE,
		sdat.command = TMP007_STATMASK,  // Status register
		sdat.size = I2C_SMBUS_WORD_DATA,
		sdat.data = &msg
	};
	int res;
	if ((res = ioctl(smbusfd, I2C_SMBUS, &sdat)) < 0) {
	  return res;  // maybe not the best value
	}	
	
	return 0;
}

int main(void){

  int fd;
  double temp;

  if ((fd = open(filename, O_RDWR)) < 0) {        // Open port for reading and writing
    perror("Failed to open i2c port");
    return -1;
  }
        
  if (ioctl(fd, I2C_SLAVE, MLX_I2CADDR) < 0) {        // Set the port options and set the address of the device we wish to speak to
    perror("Unable to get bus access to talk to slave");
    close(fd);
    return -1;
  }
  if (ioctl(fd, I2C_PEC, 1) < 0) {
    perror("PEC");
    close(fd);
    return -1;
  }
  if (MLX90614_read(fd, temp) < 0) {
    perror("Read failure");
    close(fd);
    return -1;
  }

  cout << temp << " K" << std::endl;
  
  if (ioctl(fd, I2C_SLAVE, TMP_I2CADDR) < 0) {        // Set the port options and set the address of the device we wish to speak to
    perror("Unable to get bus access to talk to slave");
    close(fd);
    return -1;
  }
  if (ioctl(fd, I2C_PEC, 1) < 0) {
    perror("PEC");
    close(fd);
    return -1;
  }
  if (TMP007_read(fd, temp) < 0) {
    perror("Read failure");
    close(fd);
    return -1;
  }

  cout << temp << " C" << std::endl;
  close(fd);
  return 0;

};
