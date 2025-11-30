/*
    gcc gdiprint.c -o gdiprint.exe -lgdi32
    gdiprint.exe "EPSON TM-T88IV Receipt" "Hello Ben"
*/

#include <windows.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <PrinterName> <StringToPrint>\n", argv[0]);
        return 1;
    }

    const char *printerName = argv[1];
    const char *text = argv[2];

    DOCINFO di = { sizeof(DOCINFO), "gdiprint.exe data", NULL };
    HDC hdc = CreateDC("WINSPOOL", printerName, NULL, NULL);

    if (!hdc) {
        fprintf(stderr, "CreateDC failed.\n");
        return 1;
    }

    if (StartDoc(hdc, &di) > 0) {
        if (StartPage(hdc) > 0) {
            TextOut(hdc, 100, 100, text, strlen(text));
            EndPage(hdc);
        }
        EndDoc(hdc);
    }

    DeleteDC(hdc);
    return 0;
}
