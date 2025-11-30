/*
    gcc imgprint.c -o imgprint.exe -lwinspool
    imgprint.exe "EPSON TM-T88IV Receipt" file.bmp
*/

#include <windows.h>
#include <winspool.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <PrinterName> <BMPFile>\n", argv[0]);
        return 1;
    }

    const char *printerName = argv[1];
    const char *bmpFile = argv[2];

    FILE *f = fopen(bmpFile, "rb");
    if (!f) { perror("fopen"); return 1; }

    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;

    fread(&bfh, sizeof(bfh), 1, f);
    fread(&bih, sizeof(bih), 1, f);

    if (bfh.bfType != 0x4D42) {
        fprintf(stderr, "Not a BMP file\n");
        fclose(f);
        return 1;
    }

    int width = bih.biWidth;
    int height = bih.biHeight;
    int bpp = bih.biBitCount;

    fseek(f, bfh.bfOffBits, SEEK_SET);
    unsigned char *pixels = malloc(width * height * (bpp/8));
    fread(pixels, 1, width * height * (bpp/8), f);
    fclose(f);

    // Convert to 1-bit packed rows (assume 24-bit input for simplicity)
    int bytesPerRow = (width + 7) / 8;
    unsigned char *raster = calloc(bytesPerRow * height, 1);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * (bpp/8);
            unsigned char r = pixels[idx+2];
            unsigned char g = pixels[idx+1];
            unsigned char b = pixels[idx+0];
            int gray = (r+g+b)/3;
            int bit = (gray < 128); // black if dark
            if (bit) raster[y*bytesPerRow + (x>>3)] |= (0x80 >> (x&7));
        }
    }
    free(pixels);

    // ESC/POS raster command header
    unsigned char header[8];
    header[0] = 0x1D; header[1] = 0x76; header[2] = 0x30; header[3] = 0; // m=0
    header[4] = bytesPerRow & 0xFF;
    header[5] = (bytesPerRow >> 8) & 0xFF;
    header[6] = height & 0xFF;
    header[7] = (height >> 8) & 0xFF;

    HANDLE hPrinter;
    DOC_INFO_1 docInfo;
    DWORD dwWritten;

    if (!OpenPrinter((LPSTR)printerName, &hPrinter, NULL)) {
        fprintf(stderr, "Failed to open printer\n");
        return 1;
    }

    docInfo.pDocName = "imgprint.exe image data";
    docInfo.pOutputFile = NULL;
    docInfo.pDatatype = "RAW";

    if (StartDocPrinter(hPrinter, 1, (LPBYTE)&docInfo)) {
        if (StartPagePrinter(hPrinter)) {
            WritePrinter(hPrinter, header, sizeof(header), &dwWritten);
            WritePrinter(hPrinter, raster, bytesPerRow*height, &dwWritten);
            EndPagePrinter(hPrinter);
        }
        EndDocPrinter(hPrinter);
    }

    ClosePrinter(hPrinter);
    free(raster);
    return 0;
}
