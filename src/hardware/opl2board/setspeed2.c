/*
 * tio - a simple TTY terminal I/O application
 *
 * Copyright (c) 2017  Martin Lund
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <sys/ioctl.h>
#include <asm-generic/ioctls.h>
#include <asm-generic/termbits.h>

#include <unistd.h> // write(), read(), close()

#include <string.h>

void makeraw(struct termios2* termios2_p, int* fd){
	
	ioctl(*fd, TCGETS2, termios2_p);
	/*
	termios2_p->c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
	termios2_p->c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
	termios2_p->c_cflag &= ~CSIZE; // Clear all bits that set the data size 
	termios2_p->c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
	termios2_p->c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
	termios2_p->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
	termios2_p->c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	termios2_p->c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	termios2_p->c_cc[VTIME] = 1;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	termios2_p->c_cc[VMIN] = 0;
	//*/		
	termios2_p->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXOFF | IXANY);
	termios2_p->c_oflag &= ~OPOST;
	termios2_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN | ECHOE);
	termios2_p->c_cflag &= ~(CSIZE | PARENB);
	termios2_p->c_cflag |= CS8;
	
	ioctl(*fd, TCSETS2, termios2_p);
}

int setspeed2(int fd, int baudrate){
    struct termios2 tty;
    int status;

    status = ioctl(fd, TCGETS2, &tty);

	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXOFF | IXANY);
	tty.c_oflag &= ~OPOST;
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN | ECHOE);
	tty.c_cflag &= ~(CSIZE | PARENB | CBAUD);
	tty.c_cflag |= CS8|BOTHER;//*/
    // Set baudrate speed using termios2 interface
    //tty.c_cflag &= ~CBAUD;
    //tty.c_cflag |= BOTHER;
    tty.c_ispeed = baudrate;
    tty.c_ospeed = baudrate;
    
    status = ioctl(fd, TCSETS2, &tty);

    return status;
}

size_t commPortwrite(int* fd, const void *buf, size_t count){
	return write(*fd, buf, count);
}

int readPort(int* serial_port){
	char read_buf [256];

  // Normally you wouldn't do this memset() call, but since we will just receive
  // ASCII data for this example, we'll set everything to 0 so we can
  // call printf() easily.
  //memset(&read_buf, '\0', sizeof(read_buf));

  // Read bytes. The behaviour of read() (e.g. does it block?,
  // how long does it block for?) depends on the configuration
  // settings above, specifically VMIN and VTIME
  int num_bytes = read(*serial_port, &read_buf, sizeof(read_buf));
  return num_bytes;
}
