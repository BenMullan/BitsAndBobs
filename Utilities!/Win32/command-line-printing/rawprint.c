/*
    gcc rawprint.c -o rawprint.exe -lwinspool
    rawprint.exe "EPSON TM-T88IV Receipt" "Hello Ben\n\x1B@\x1DVA0"
*/

/*
    ref:    https://download4.epson.biz/sec_pubs/pos/reference_en/escpos/tmt88iv.html
    -
    reset:  rawprint.exe "EPSON TM-T88IV Receipt" "\x1B@"
    cut:    rawprint.exe "EPSON TM-T88IV Receipt" "\x1DVA0"
    bold:   rawprint.exe "EPSON TM-T88IV Receipt" "\x1BE\x01Hello Bold\x0A\x1BE\x00Normal again\x0A"
    large:  rawprint.exe "EPSON TM-T88IV Receipt" "\x1D!0Hello Normal\x0A\x1D!0x11Double Size\x0A\x1D!0Back to Normal\x0A"
    under:  rawprint.exe "EPSON TM-T88IV Receipt" "\x1B-\x01Underlined\x0A\x1B-\x00Not underlined\x0A"    
    -
    [Command]       [Bytes]                 [Meaning]
    Initialize      ESC @ (\x1B@)           Reset printer
    Line feed       LF (\x0A)               Advance paper one line
    Cut paper       GS V A 0 (\x1DVA0)      Full cut
    Partial cut     GS V A 1 (\x1DVA1)      Partial cut
    Bold on/off     ESC E n (\x1BE\x01 / \x1BE\x00) Toggle bold
    Underline       ESC - n (\x1B-\x01)     Toggle underline
    Font size       GS ! n (\x1D!n)         Double‑width/height
    Alignment       ESC a n (\x1Ba0/1/2)    Left/center/right
    Print barcode   GS k m d (\x1Dk...)     Various barcode types
    Print image     GS v 0 (\x1Dv0)         Raster bit image
    -
    complex example:
    rawprint.exe "EPSON TM-T88IV Receipt" "\x1B@Hello Ben\n\x1Ba1\x1D!\x11MY STORE NAME\x0A\x1D!\x00\x1Ba0----------------------------------------\x0AItem A                 2.50\x0AItem B                 1.75\x0AItem C                 3.25\x0A----------------------------------------\x0A\x1BE\x01Subtotal              7.50\x0A\x1BE\x00\x1D!\x11TOTAL                7.50\x0A\x1D!\x00----------------------------------------\x0AThank you for shopping!\x0A\x1Ba1Scan below for offers\x0A\x1Ba0\x1Dk4\x05ABCDE\x0A\x1DVA0"
*/

#include <windows.h>
#include <winspool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int parseEscapes(const char *in, unsigned char *out) {
    int i = 0, j = 0;
    while (in[i]) {
        if (in[i] == '\\') {
            i++;
            switch (in[i]) {
                case 'n': out[j++] = '\n'; i++; break;
                case 'r': out[j++] = '\r'; i++; break;
                case 't': out[j++] = '\t'; i++; break;
                case '\\': out[j++] = '\\'; i++; break;
                case 'x': {
                    char buf[3] = { in[i+1], in[i+2], 0 };
                    out[j++] = (unsigned char)strtol(buf, NULL, 16);
                    i += 3;
                    break;
                }
                default: out[j++] = in[i++]; break;
            }
        } else {
            out[j++] = in[i++];
        }
    }
    return j;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <PrinterName> <StringWithEscapes>\n", argv[0]);
        return 1;
    }

    const char *printerName = argv[1];
    const char *input = argv[2];
    unsigned char buffer[4096];
    int buflen = parseEscapes(input, buffer);

    HANDLE hPrinter;
    DOC_INFO_1 docInfo;
    DWORD dwWritten;

    if (!OpenPrinter((LPSTR)printerName, &hPrinter, NULL)) {
        fprintf(stderr, "Failed to open printer: %s\n", printerName);
        return 1;
    }

    docInfo.pDocName = "rawprint.exe ESC/POS data";
    docInfo.pOutputFile = NULL;
    docInfo.pDatatype = "RAW";

    if (StartDocPrinter(hPrinter, 1, (LPBYTE)&docInfo)) {
        if (StartPagePrinter(hPrinter)) {
            WritePrinter(hPrinter, buffer, buflen, &dwWritten);
            EndPagePrinter(hPrinter);
        }
        EndDocPrinter(hPrinter);
    }

    ClosePrinter(hPrinter);
    return 0;
}
