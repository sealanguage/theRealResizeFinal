#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: resize n infile outfile\n");
        return 1;
    }

    // remember filenames
    // store number as number not char
    int n = atoi(argv[1]);
        if (n < 1 || n > 100)
        {
            fprintf(stderr, "number n needs to be between 1 and 100\n");
            return 0;
        }
    char *infile = argv[2];
    char *outfile = argv[3];

    // setup variables for padding of the infile and padding of the outfile
    int inpad = 0;
    int outpadding = 0;


    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    // error message and return if file fails to open
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // determine padding for scanlines
    inpad =  (4 -(bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;  // make sure this refers to the old width
    printf("infile padding inpad is  %d\n", inpad);

    // structs for new bitmapfileheader
    BITMAPFILEHEADER OUTbf = bf;
    BITMAPINFOHEADER OUTbi = bi;

    // print width and height of the OUTFILE
    OUTbi.biWidth *= n;
    OUTbi.biHeight *= n;
    outpadding =  (4 - (OUTbi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    int pixcounter = 0;

    printf("OUTbi.biWidth  value for outfile is: %i\n", OUTbi.biWidth);
    printf("abs(OUTbi.biHeight) for outfile value is: %i\n", abs(OUTbi.biHeight));

    // edit here to account for outfile's BITMAPINFOHEADERs resize
    OUTbi.biSizeImage = ((sizeof(RGBTRIPLE) * OUTbi.biWidth) + outpadding) * abs(OUTbi.biHeight);
    printf("OUTbi.biSizeImage value is: %i\n", bi.biSizeImage);

    OUTbf.bfSize = OUTbi.biSizeImage + sizeof(OUTbf) + sizeof(OUTbi);
    printf("int bf.bfSize FIRST %d\n", bf.bfSize);
    printf("infile padding is %d byte\n", inpad);
    printf("outfile padding is %d byte\n", outpadding);

    // write outfile's BITMAPFILEHEADER
    fwrite(&OUTbf, sizeof(BITMAPFILEHEADER), 1, outptr);   // this should be 14

    // write outfile's BITMAPINFOHEADER
    fwrite(&OUTbi, sizeof(BITMAPINFOHEADER), 1, outptr);    // this should be 40

// this is the copy.c loop structure for duplicating the pixels
// iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        for (int vertical = 0; vertical < n; vertical++)
            {
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);


                    for (int pixel = 0; pixel < n; pixel++)  // inbiWidth
                        {
                            // write RGB triple to outfile
                            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                            // needs to increment the pixels n times
                            pixcounter++;
                        }
            }

            // skip over padding, if any
            fseek(inptr, inpad, SEEK_CUR);

            // then add it back (to demonstrate how)
            for (int k = 0; k < outpadding; k++)
            {
                //adding padding back into the image
                fputc(0x00, outptr);
            }

            // print rows n number of times to get vertical duplication
            if (vertical < n - 1)
                {
                    fseek(inptr, - (bi.biWidth * 3 + inpad), SEEK_CUR );
                }
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}



