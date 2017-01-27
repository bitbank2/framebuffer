#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <math.h>

uint32_t ulColors[8] = {0, 0x0000ff, 0x00ff00, 0xff0000, 0xffff, 0xff00ff, 0xffff00, 0xffffff};

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : MilliTime()                                                *
 *               returns the relative time in milliseconds                  *
 *                                                                          *
 ****************************************************************************/
int MilliTime()
{
int iTime;
struct timespec res;

    clock_gettime(CLOCK_MONOTONIC, &res);
    iTime = 1000*res.tv_sec + res.tv_nsec/1000000;

    return iTime;
} /* MilliTime() */

// draw a circle
void DrawCircle(uint32_t *pFB, int iWidth, int iRadius, int iXCenter, int iYCenter, uint32_t ulColor)
{
float angle;
int x, y;

   for (angle=0.0; angle<6.28; angle += 0.01)
   {
	x = (int)(cos(angle) * iRadius) + iXCenter;
	y = (int)(sin(angle) * iRadius) + iYCenter;
	if (x >= 0 && x < iWidth && y >= 0)
	{
		pFB[(y*iWidth) + x] = ulColor;
	}
   }
} /* DrawCircle() */

void DrawFilledCircle(uint32_t *pFB, int iWidth, int iRadius, int iXCenter, int iYCenter, uint32_t ulColor)
{
float angle;
int x, y, xp, x1, y1;
uint32_t *ptemp;

   for (angle=-3.14/2; angle<=3.14/2; angle += 0.05)
   {
	x = (int)(cos(angle) * iRadius);
	y = (int)(sin(angle) * iRadius);
//	x1 = x + iXCenter;
//	y1 = y + iYCenter;
	{
//		printf("x=%d, y=%d", x, y);
		ptemp = &pFB[((y+iYCenter)*iWidth) + iXCenter-x];
		for(xp=iXCenter-x; xp<iXCenter+x; xp++)
		{
			*ptemp ++= ulColor;
//			pFB[((y+iYCenter)*iWidth) + xp] = ulColor;
		}
	}
   }
} /* DrawFilledCircle() */

int main(int argc, char* argv[])
{
  int fbfd = 0;
//  uint32_t *p;
//  unsigned char *s;
  int i, j;
  int iTime;
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;
  long int screensize = 0;
  char *fbp = 0;

  // Open the file for reading and writing
  fbfd = open("/dev/fb0", O_RDWR);
  if (!fbfd) {
    printf("Error: cannot open framebuffer device.\n");
    return(1);
  }
  printf("The framebuffer device was opened successfully.\n");

  // Get fixed screen information
  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
    printf("Error reading fixed information.\n");
  }

  // Get variable screen information
  if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
    printf("Error reading variable information.\n");
  }
  printf("%dx%d, %d bpp\n", vinfo.xres, vinfo.yres, 
         vinfo.bits_per_pixel );

  // map framebuffer to user memory 
  screensize = finfo.smem_len;

  fbp = (char*)mmap(0, 
                    screensize, 
                    PROT_READ | PROT_WRITE, 
                    MAP_SHARED, 
                    fbfd, 0);

  if ((int)fbp == -1) {
    printf("Failed to mmap.\n");
  }
  else {
    // draw...
    // just fill upper half of the screen with something
//    memset(fbp, 0xff, screensize/2);
    // and lower half with something else
//    memset(fbp + screensize/2, 0, screensize/2);
//    p = (uint32_t *)&fbp[screensize/2];
//    s = (unsigned char *)&fbp[screensize/2];
	iTime = MilliTime();
	for (j=1; j<=100; j++)
	{
		for (i=0; i<8; i++)
		{
			DrawFilledCircle((uint32_t *)fbp, vinfo.xres, 50, (i+1)*100, (i+1)*100, ulColors[i]);
		} //for i
	} // for j
	iTime = MilliTime() - iTime;
	printf("Elapsed time in ms = %d\n", iTime);
  }

  // cleanup
  munmap(fbp, screensize);
  close(fbfd);
  return 0;
}
