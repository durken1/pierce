// Default is left half. If compiling for right half uncomment next line
//#define RIGHT

#ifdef RIGHT

#define PIPE_NUMBER 1

#define R0C0 13
#define R0C1 14
#define R0C2 17
#define R0C3 18
#define R0C4 19

#define R1C0 6
#define R1C1 7
#define R1C2 8
#define R1C3 9
#define R1C4 10

#define R2C0 5
#define R2C1 4
#define R2C2 3
#define R2C3 2
#define R2C4 1

#define R3C2 00
#define R3C3 30
#define R3C4 29

#else

#define PIPE_NUMBER 0

#define R0C0 29
#define R0C1 28
#define R0C2 23
#define R0C3 22
#define R0C4 21

#define R1C0 3
#define R1C1 2
#define R1C2 1
#define R1C3 0
#define R1C4 30

#define R2C0 4
#define R2C1 5
#define R2C2 6
#define R2C3 7
#define R2C4 8

#define R3C2 9
#define R3C3 10
#define R3C4 13

#endif

#define ROWS 4

#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}
