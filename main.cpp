//****************************************************
// Sample code to display a .bmp file
//
// g++ main.cpp -lglut -lGLU -lGL
//
//****************************************************



#include <stdio.h>
#include "GL/freeglut.h"
#include "GL/gl.h" 

#pragma pack(2) /***** 2 byte aligned not 4 byte aligned *****/

typedef struct                       /**** BMP file header structure ****/
{
    unsigned short bfType;           /* Magic number for file */
    unsigned int   bfSize;           /* Size of file */
    unsigned short bfReserved1;      /* Reserved */
    unsigned short bfReserved2;      /* ... */
    unsigned int   bfOffBits;        /* Offset to bitmap data */
} BITMAPFILEHEADER;

#pragma pack()  /***** return to 4 byte aligned *****/

#  define BF_TYPE 0x4D42             /* "MB" */

typedef struct                       /**** BMP file info structure ****/
{
    unsigned int   biSize;           /* Size of info header */
    int            biWidth;          /* Width of image */
    int            biHeight;         /* Height of image */
    unsigned short biPlanes;         /* Number of color planes */
    unsigned short biBitCount;       /* Number of bits per pixel */
    unsigned int   biCompression;    /* Type of compression to use */
    unsigned int   biSizeImage;      /* Size of image data */
    int            biXPelsPerMeter;  /* X pixels per meter */
    int            biYPelsPerMeter;  /* Y pixels per meter */
    unsigned int   biClrUsed;        /* Number of colors used */
    unsigned int   biClrImportant;   /* Number of important colors */
} BITMAPINFOHEADER;


/*
 * Constants for the biCompression field...
 */

#  define BI_RGB       0             /* No compression - straight BGR data */
#  define BI_RLE8      1             /* 8-bit run-length compression */
#  define BI_RLE4      2             /* 4-bit run-length compression */
#  define BI_BITFIELDS 3             /* RGB bitmap with RGB masks */

typedef struct                       /**** Colormap entry structure ****/
{
    unsigned char  rgbBlue;          /* Blue value */
    unsigned char  rgbGreen;         /* Green value */
    unsigned char  rgbRed;           /* Red value */
    unsigned char  rgbReserved;      /* Reserved */
} RGBQUAD;

class Bitmap {
public:
    Bitmap(const char* filename);
    ~Bitmap();
    RGBQUAD* pixels;
    BITMAPFILEHEADER fh;
    BITMAPINFOHEADER ih;

    private:

};

void *global_bits = NULL; 
BITMAPINFOHEADER global_ih;

void *ReadBitmap(char *filename)
{
    BITMAPFILEHEADER h;
    void *bits = NULL;
    FILE *file = fopen(filename, "rb");

    if(file != NULL)
    {
        printf("sizeof(BITMAPFILEHEADER) == %lu\n", sizeof(BITMAPFILEHEADER));
        size_t x = fread(&h, 1, sizeof(BITMAPFILEHEADER), file);
        printf("bytes read == %lu\n", x);
        printf("bfOffBits == %u\n", h.bfOffBits);


        printf("sizeof(BITMAPINFOHEADER) == %lu\n", sizeof(BITMAPINFOHEADER));
        size_t zz = fread(&global_ih, 1, sizeof(BITMAPINFOHEADER), file);
        printf("bytes read == %lu\n", zz);
        printf("width == %d\n", global_ih.biWidth);
        printf("height == %d\n", global_ih.biHeight);
        printf("biPlanes == %u\n", global_ih.biPlanes);
        printf("biBitCount == %u\n", global_ih.biBitCount);
        printf("biCompression == %u\n", global_ih.biCompression);
        printf("biSizeImage = %u\n", global_ih.biSizeImage);

        int checksize = global_ih.biWidth * global_ih.biHeight * 3;
        printf("checksize = %d\n", checksize);

        int fseekstatus = fseek(file, h.bfOffBits, SEEK_SET);
        if (fseekstatus == 0)
           printf("good fseek\n");
        else
        {
           printf("bad fseek\n");
           fclose(file);
           return NULL;
        }

        bits = malloc(global_ih.biSizeImage);
        if (bits == NULL)
        {
           printf("bad malloc\n");
           fclose(file);
           return NULL;
        }
        else
           printf("good malloc\n");

        size_t bytesread = fread(bits, 1, global_ih.biSizeImage, file);
        
        fclose(file);

        if (bytesread == global_ih.biSizeImage)
        {
            printf("good fread\n");
            return bits;
        }
        else
        {
            printf("bad fread bytesread == %lu\n", bytesread);
            free(bits);
            return NULL;
        }
    }

    printf("failed to open file\n");
    return NULL;
}




void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
 
 
	glDrawPixels( global_ih.biWidth, global_ih.biHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, global_bits );
	glEnd();
	glFlush();
}
 
void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}
 
main (int argc, char **argv)
{

        if (argc != 2)
        {
           printf("usage: ./a.out <.bmp file>\n");
           return 0;
        }
 
        global_bits = ReadBitmap(argv[1]);
        if (global_bits == NULL)
        {
            printf("bad ReadBitmap\n");
            return 0;
        }

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
        glutInitWindowSize(1210,605);
        glutInitWindowPosition(0,0);
        glutCreateWindow("opengl_sample_bitmap");
        init();
        glutDisplayFunc(display);
        glutMainLoop();

        return 0;
}
