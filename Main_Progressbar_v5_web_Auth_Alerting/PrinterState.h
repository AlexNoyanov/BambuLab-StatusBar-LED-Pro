#ifndef PRINTER_STATE_H
#define PRINTER_STATE_H

enum PrinterState { Connecting,
                    Heating,
                    Loading,
                    Printing,
                    Finished,
                    Error,
                    Paused };

#endif // PRINTER_STATE_H
