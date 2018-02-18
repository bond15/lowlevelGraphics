//#include "stdio.h"
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <linux/fb.h>
//#include <sys/ioctl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

//helper functions

uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo * vinfo)
{
	return (r << vinfo->red.offset) | (g << vinfo->green.offset) | (b << vinfo->blue.offset) ;
}

int main()
{
	//frame buffer file descriptor
	int fb_fd = open("/dev/fb0",O_RDWR);

	struct fb_fix_screeninfo finfo;  
	struct fb_var_screeninfo vinfo;
	
	// iocrl  - io control, manages special device files
	
	//frame buffer io get variable screen info
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
	//frame buffer io get fixed screen info
	ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

	//change some parameters (because variable information can change, not fixed)
	vinfo.grayscale = 0;
	vinfo.bits_per_pixel=32;
	
	//update info with put, then recollect with get
	ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
	
	//calculate screen size (in bytes)
	long screensize = vinfo.yres_virtual * finfo.line_length;

	// see man mmap - we are mapping the buffer to memory
	uint8_t * fbp = mmap(0,screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);
	
	// now draw!
	for (int x = 0; x < vinfo.xres; x++)
		for (int y = 0; y < vinfo.yres; y++)
		{
			long location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8)  + (y + vinfo.yoffset) * finfo.line_length;
			*((uint32_t *) (fbp + location)) = pixel_color(0xFF, 0x00, 0xFF, &vinfo);
		}
	
}
