#ifndef _def_h_
#define _def_h_

#define CLOCK_NTSC 53693175
#define CLOCK_PAL  53203424

#define CLOCK_NTSC 53693175
#define CLOCK_PAL  53203424

#define GARBAGE_CODE(a, b, c, d) GARBAGE_CODE_##a() GARBAGE_CODE_##b() GARBAGE_CODE_##c() GARBAGE_CODE_##d()

static int g_0 = 0;
static int g_1 = 0;
static int g_2 = 0;
static int g_3 = 0;
static int g_4 = 0;
static int g_5 = 0;
static int g_6 = 0;
static int g_7 = 0;

#define GARBAGE_CODE_0() { if(g_0){ printf("GARBAGE_CODE_0"); long i=0; float j=7.2; double k=3.1; k = i + j * 32 / 10; while(k<109){ i=32+j; } }  }
#define GARBAGE_CODE_1() { if(g_1){ printf("GARBAGE_CODE_1"); int a=1; int b=3; int c=1; c = a+b*32/10; while(c<109){ a=32+a; } } }
#define GARBAGE_CODE_2() { if(g_2){ printf("GARBAGE_CODE_2"); double i=1.1; float j=2; int k=0; k = i + j * 32 / 10; while(k<109){ i=32+j; } } }
#define GARBAGE_CODE_3() { if(g_3){ printf("GARBAGE_CODE_3"); int w=0; char x=0; float y=1; y = w + x * 32 / 10; while(x<109){ w=32+x; } }  }
#define GARBAGE_CODE_4() { if(g_4){ printf("GARBAGE_CODE_4"); int i=0; float j=0; char k=6; k = i + j * 32 / 10; while(k<109){ i=32+j; } }  }
#define GARBAGE_CODE_5() { if(g_5){ printf("GARBAGE_CODE_5"); double a=0; int b=0; int c=2; c = a+b*32/10; while(c<109){ a=32+a; } } }
#define GARBAGE_CODE_6() { if(g_6){ printf("GARBAGE_CODE_6"); short i=0; float j=0; int k=0; k = i + j * 32 / 10; while(k<109){ i=32+j; } } }
#define GARBAGE_CODE_7() { if(g_7){ printf("GARBAGE_CODE_7"); int w=0; float x=5; float y=7.1; y = w + x * 32 / 10; for(x=0;x<109;x++){ w=32+x; } }  }

#endif