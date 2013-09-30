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
 
int address = 0x5a;
const char *filename = "/dev/i2c-1";
double tempfactor = 0.02;

int MLX90614_read(int smbusfd, double temp) {
	union i2c_smbus_data msg;
	struct i2c_smbus_ioctl_data sdat = {
		sdat.read_write = I2C_SMBUS_READ,
		sdat.command = 7,  // read register 7, ir
		sdat.size = I2C_SMBUS_WORD_DATA,
		sdat.data = &msg
	};
	int res;
	if ((res = ioctl(smbusfd, I2C_SMBUS, &sdat)) < 0) {
	  return res;  // maybe not the best value
	}
	temp = tempfactor * (double)msg.word;
	
	return 0;
}

int main(void){

  int fd;
  double temp;

  if ((fd = open(filename, O_RDWR)) < 0) {        // Open port for reading and writing
    perror("Failed to open i2c port");
    return -1;
  }
        
  if (ioctl(fd, I2C_SLAVE, address) < 0) {        // Set the port options and set the address of the device we wish to speak to
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
    perror("Read failure.");
    close(fd);
    return -1;
  }

  cout << temp << " K" << std::endl;
  close(fd);
  return 0;

};
