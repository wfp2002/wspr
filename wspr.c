/*

 Raspberry Pi bareback LF/MF/HF/VHF WSPR transmitter  <pe1nnz@amsat.org>

 Makes a very simple WSPR beacon from your RasberryPi by connecting GPIO 
 port to Antanna (and LPF), operates on LF, MF, HF and VHF bands from 
 0 to 250 MHz.

License:
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

Credits:
  Credits goes to Oliver Mattos and Oskar Weigl who implemented PiFM [1]
  based on the idea of exploiting RPi DPLL as FM transmitter. Dan MD1CLV
  combined this effort with WSPR encoding algorithm from F8CHK, resulting  
  in WsprryPi a WSPR beacon for LF and MF bands. Guido PE1NNZ extended 
  this effort with DMA based PWM modulation of fractional divider that was 
  part of PiFM, allowing to operate the WSPR beacon also on HF and VHF bands.
  In addition time-synchronisation and double amount of power output was 
  implemented. 

  [1] PiFM code from http://www.icrobotics.co.uk/wiki/index.php/Turning_the_Raspberry_Pi_Into_an_FM_Transmitter

To use:
  In order to transmit legally, a HAM Radio License is REQUIRED for running 
  this experiment. The output is a square wave so a low pass filter is REQUIRED.
  Connect a low-pass filter (via decoupling C) to GPIO4 (GPCLK0) and Ground pin
  of your Raspberry Pi, connect an antenna to the LPF. The GPIO4 and GND pins
  are found on header P1 pin 7 and 9 respectively, the pin closest to P1 label 
  is pin 1 and its 3rd and 4th neighbour is pin 7 and 9 respectively, see this 
  link for pin layout: http://elinux.org/RPi_Low-level_peripherals Examples of 
  low-pass filters can be found here: http://www.gqrp.com/harmonic_filters.pdf
  The expected power output is 10mW (+10dBm) in a 50 Ohm load. This looks
  neglible, but when connected to a simple dipole antenna this may result in 
  reception reports ranging up to several thousands of kilometers.
  Example of low-pass filters here: http://www.gqrp.com/harmonic_filters.pdf
  As the Raspberry Pi does not attenuate ripple and noise components from the 
  5V USB power supply, it is RECOMMENDED to use a regulated supply that has
  sufficient ripple supression. Supply ripple might be seen as mixing products
  products centered around the transmit carrier typically at 100/120Hz.

  This software is using system time to determine the start of a WSPR 
  transmissions, so keep the system time synchronised within 1sec precision, 
  i.e. use NTP network time synchronisation or set time manually with date 
  command. A WSPR broadcast starts on even minute and takes 2 minutes for WSPR-2 
  or starts at :00,:15,:30,:45 and takes 15 minutes for WSPR-15. It contains 
  a callsign, 4-digit Maidenhead square locator and transmission power.
  Reception reports can be viewed on Weak Signal Propagation Reporter Network 
  at: http://wsprnet.org/drupal/wsprnet/spots 

  Frequency calibration is REQUIRED to ensure that the WSPR-2 transmission occurs
  within the 200 Hz narrow band. The reference crystal on your RPi might have
  an frequency error (which in addition is temp. dependent -1.3Hz/degC @10MHz).
  To calibrate, the frequency might be manually corrected on the command line 
  or by changing the F_XTAL value in the code. A practical way to calibrate 
  is to tune the transmitter on the same frequency of a medium wave AM broadcast 
  station; keep tuning until zero beat (the constant audio tone disappears when 
  the transmitter is exactly on the same frequency as the broadcast station),
  and determine the frequency difference with the broadcast station. This is 
  the frequency error that can be applied for correction while tuning on a WSPR
  frequency.

  DO NOT expose GPIO4 to voltages or currents that are above the specified
  Absolute Maximum limits. GPIO4 outputs a digital clock in 3V3 logic, with a
  maximum current of 16mA. As there is no current protection available and
  a DC component of 1.6V, DO NOT short-circuit or place a resistive (dummy) load
  straight on the GPIO4 pin, as it may draw too much current. Instead, use a
  decoupling capacitor to remove DC component when connecting the output
  dummy loads, transformers, antennas, etc. DO NOT expose GPIO4 to electro-
  static voltages or voltages exceeding the 0 to 3.3V logic range; connecting an
  antenna directly to GPIO4 may damage your RPi due to transient voltages such as
  lightning or static buildup as well as RF from other transmitters operating into
  nearby antennas. Therefore it is RECOMMENDED to add some form of isolation, e.g.
  by using a RF transformer, a simple buffer/driver/PA stage, two schottky small 
  signal diodes back to back.

Installation / update:
  Open a terminal and execute the following commands:
   sudo apt-get install git
   rm -rf WsprryPi
   git clone https://github.com/threeme3/WsprryPi.git
   cd WsprryPi

Usage: 
  sudo ./wspr <[prefix]/callsign[/suffix]> <locator> <power in dBm> [<frequency in Hz> ...]
        e.g.: sudo ./wspr PA/K1JT JO21 10 7040074 0 0 10140174 0 0
        where 0 frequency represents a interval for which TX is disabled,
        wspr-2 or wspr-15 mode selection based on specified frequency.

  WSPR is used on the following frequencies (local restriction may apply):
     LF   137400 - 137600
          137600 - 137625 (WSPR-15)
     MF   475600 - 475800
          475800 - 475825 (WSPR-15)
    160m  1838000 - 1838200
          1838200 - 1838225 (WSPR-15)
     80m  3594000 - 3594200
     60m  5288600 - 5288800
     40m  7040000 - 7040200
     30m  10140100 - 10140300
     20m  14097000 - 14097200
     17m  18106000 - 18106200
     15m  21096000 - 21096200
     12m  24926000 - 24926200
     10m  28126000 - 28126200
      6m  50294400 - 50294600
      4m  70092400 - 70092600
      2m  144490400 -144490600

Compile:
  sudo apt-get install gcc
  gcc -lm wspr.c -owspr

Reference documentation:
  http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
  http://www.scribd.com/doc/127599939/BCM2835-Audio-clocks
  http://www.scribd.com/doc/101830961/GPIO-Pads-Control2
  https://github.com/mgottschlag/vctools/blob/master/vcdb/cm.yaml
  https://www.kernel.org/doc/Documentation/vm/pagemap.txt

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <math.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <malloc.h>
#include <time.h>

#define F_XTAL       (19229581.050215044276577479844352)             // calibrated 19.2MHz XTAL frequency 
#define F_PLLD_CLK   (26.0 * F_XTAL)                                 // 500MHz PLLD reference clock 

#define N_ITER  1400  // number of PWM operations per symbol; larger values gives less spurs at the cost of frequency resolution; e.g. use 22500 for HF usage up to 30MHz, 12000 up to 50MHz, 1600 for VHF usage up to 144 Mhz, F_PWM_CLK needs to be adjusted when changing N_ITER 
//#define F_PWM_CLK    (31500000.0)   // 31.5MHz PWM clock   use with N_ITER=22500
#define F_PWM_CLK    (33970588.235294117647058823529413)   // 31.5MHz calibrated PWM clock   use with N_ITER=1400

#define WSPR_SYMTIME (8192.0/12000.0)  // symbol time

#define POLYNOM_1 0xf2d05351    // polynoms for
#define POLYNOM_2 0xe4613c47    // parity generator

/* RF code: */

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int  mem_fd;
char *gpio_mem, *gpio_map;
char *spi0_mem, *spi0_map;


// I/O access
volatile unsigned *gpio = NULL;
volatile unsigned *allof7e = NULL;

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0
#define GPIO_GET *(gpio+13) // sets   bits which are 1 ignores bits which are 0

#define ACCESS(base) *(volatile int*)((int)allof7e+base-0x7e000000)
#define SETBIT(base, bit) ACCESS(base) |= 1<<bit
#define CLRBIT(base, bit) ACCESS(base) &= ~(1<<bit)
#define CM_GP0CTL (0x7e101070)
#define GPFSEL0 (0x7E200000)
#define PADS_GPIO_0_27  (0x7e10002c)
#define CM_GP0DIV (0x7e101074)
#define CLKBASE (0x7E101000)
#define DMABASE (0x7E007000)
#define PWMBASE  (0x7e20C000) /* PWM controller */

struct GPCTL {
    char SRC         : 4;
    char ENAB        : 1;
    char KILL        : 1;
    char             : 1;
    char BUSY        : 1;
    char FLIP        : 1;
    char MASH        : 2;
    unsigned int     : 13;
    char PASSWD      : 8;
};

void getRealMemPage(void** vAddr, void** pAddr) {
    void* a = (void*)valloc(4096);

    ((int*)a)[0] = 1;  // use page to force allocation.

    mlock(a, 4096);  // lock into ram.

    *vAddr = a;  // yay - we know the virtual address

    unsigned long long frameinfo;

    int fp = open("/proc/self/pagemap", 'r');
    lseek(fp, ((int)a)/4096*8, SEEK_SET);
    read(fp, &frameinfo, sizeof(frameinfo));

    *pAddr = (void*)((int)(frameinfo*4096));
}

void freeRealMemPage(void* vAddr) {

    munlock(vAddr, 4096);  // unlock ram.

    free(vAddr);
}

struct CB {
    volatile unsigned int TI;
    volatile unsigned int SOURCE_AD;
    volatile unsigned int DEST_AD;
    volatile unsigned int TXFR_LEN;
    volatile unsigned int STRIDE;
    volatile unsigned int NEXTCONBK;
    volatile unsigned int RES1;
    volatile unsigned int RES2;

};

struct DMAregs {
    volatile unsigned int CS;
    volatile unsigned int CONBLK_AD;
    volatile unsigned int TI;
    volatile unsigned int SOURCE_AD;
    volatile unsigned int DEST_AD;
    volatile unsigned int TXFR_LEN;
    volatile unsigned int STRIDE;
    volatile unsigned int NEXTCONBK;
    volatile unsigned int DEBUG;
};

struct PageInfo {
    void* p;  // physical address
    void* v;   // virtual address
};

struct PageInfo constPage;
struct PageInfo instrPage;
struct PageInfo instrs[1024];

double fracs[1024];

void txon()
{
    if(allof7e == NULL){
      allof7e = (unsigned *)mmap(
                  NULL,
                  0x01000000,  //len
                  PROT_READ|PROT_WRITE,
                  MAP_SHARED,
                  mem_fd,
                  0x20000000  //base
              );
      if ((int)allof7e==-1) exit(-1);
    }

    SETBIT(GPFSEL0 , 14);
    CLRBIT(GPFSEL0 , 13);
    CLRBIT(GPFSEL0 , 12);

    // Set GPIO drive strength, more info: http://www.scribd.com/doc/101830961/GPIO-Pads-Control2 
    //ACCESS(PADS_GPIO_0_27) = 0x5a000018 + 0;  //2mA -3.4dBm
    //ACCESS(PADS_GPIO_0_27) = 0x5a000018 + 1;  //4mA +2.1dBm
    //ACCESS(PADS_GPIO_0_27) = 0x5a000018 + 2;  //6mA +4.9dBm
    //ACCESS(PADS_GPIO_0_27) = 0x5a000018 + 3;  //8mA +6.6dBm(default)
    //ACCESS(PADS_GPIO_0_27) = 0x5a000018 + 4;  //10mA +8.2dBm
    //ACCESS(PADS_GPIO_0_27) = 0x5a000018 + 5;  //12mA +9.2dBm
    //ACCESS(PADS_GPIO_0_27) = 0x5a000018 + 6;  //14mA +10.0dBm
    ACCESS(PADS_GPIO_0_27) = 0x5a000018 + 7;  //16mA +10.6dBm

    struct GPCTL setupword = {6/*SRC*/, 1, 0, 0, 0, 1,0x5a};
    ACCESS(CM_GP0CTL) = *((int*)&setupword);
}

void txoff()
{
    struct GPCTL setupword = {6/*SRC*/, 0, 0, 0, 0, 1,0x5a}; 
    ACCESS(CM_GP0CTL) = *((int*)&setupword); 
}

void setfreq(long freq)
{
    ACCESS(CM_GP0DIV) = (0x5a << 24) + freq;
}

void txSym(int sym, double tsym)
{
    int bufPtr=0;
    int clocksPerIter = (int)((F_PWM_CLK/((double)N_ITER)) * tsym);
    //printf("tsym=%f iter=%u clocksPerIter=%u tsymerr=%f\n", tsym, N_ITER, clocksPerIter, tsym - ((float)clocksPerIter*(float)N_ITER)/F_PWM_CLK );
    int i = sym*3 + 511;
    double dval = -1.0 * fracs[i] - 0.5; // ratio between -0.5 and 0.5 of frequency position that is in between two fractional clock divider bins (frequency goes up for dval from -0.5 to 0.5)
    int k = (int)(round(dval));  // integer component
    double frac = (dval - (double)k)/2 + 0.5;
    unsigned int fracval = (frac*clocksPerIter);
    //printf("i=%d *i=%u %u fracval=%u dval=%f sym=%d\n", i, ((int*)(constPage.v))[i-1], ((int*)(constPage.v))[i+1], fracval, dval, sym); 
    int j;
    for(j=0; j!=N_ITER; j++){
        bufPtr++;
        while( ACCESS(DMABASE + 0x04 /* CurBlock*/) ==  (int)(instrs[bufPtr].p)) usleep(100);
        ((struct CB*)(instrs[bufPtr].v))->SOURCE_AD = (int)constPage.p + (i-1)*4;

        bufPtr++;
        while( ACCESS(DMABASE + 0x04 /* CurBlock*/) ==  (int)(instrs[bufPtr].p)) usleep(100);
        ((struct CB*)(instrs[bufPtr].v))->TXFR_LEN = clocksPerIter-fracval;

        bufPtr++;
        while( ACCESS(DMABASE + 0x04 /* CurBlock*/) ==  (int)(instrs[bufPtr].p)) usleep(100);
        ((struct CB*)(instrs[bufPtr].v))->SOURCE_AD = (int)constPage.p + (i+1)*4;

        bufPtr=(bufPtr+1) % (1024);
        while( ACCESS(DMABASE + 0x04 /* CurBlock*/) ==  (int)(instrs[bufPtr].p)) usleep(100);
        ((struct CB*)(instrs[bufPtr].v))->TXFR_LEN = fracval;
    }
}

void unSetupDMA(){
    printf("exiting\n");
    struct DMAregs* DMA0 = (struct DMAregs*)&(ACCESS(DMABASE));
    DMA0->CS =1<<31;  // reset dma controller
    txoff(); 
}

void handSig() {
  exit(0);
}
void setupDMATab( float centerFreq, double symOffset, double tsym, int nsym ){
   // make data page contents - it's essientially 1024 different commands for the
   // DMA controller to send to the clock module at the correct time.
  int i;
  for(i=1; i<1023; i+=3){
     double freq = centerFreq + ((double)(-511 + i))*symOffset/3.0;
     double divisor = F_PLLD_CLK/freq;
     unsigned long integer_part = (unsigned long) divisor;
     unsigned long fractional_part = (divisor - integer_part) * (1 << 12);
     unsigned long tuning_word = (0x5a << 24) + integer_part * (1 << 12) + fractional_part;
     if(fractional_part == 0 || fractional_part == 1023){
       if((-511 + i) >= 0 && (-511 + i) <= (nsym * 3)) 
         printf("warning: symbol %u unusable because fractional divider is out of range, try near frequency.\n", i/3);
     }
     ((int*)(constPage.v))[i-1] = tuning_word - 1;
     ((int*)(constPage.v))[i] = tuning_word;
     ((int*)(constPage.v))[i+1] = tuning_word + 1;
     double actual_freq = F_PLLD_CLK/((double)integer_part + (double)fractional_part/(double)(1<<12));
     double freq_corr = freq - actual_freq;
     double delta = F_PLLD_CLK/((double)integer_part + (double)fractional_part/(double)(1<<12)) - F_PLLD_CLK/((double)integer_part + ((double)fractional_part+1.0)/(double)(1<<12));
     int clocksPerIter = (int)((F_PWM_CLK/((double)N_ITER)) * tsym);
     double resolution = 2.0 * delta / ((double)clocksPerIter);
     if(resolution > symOffset ){
       printf("warning: PWM/PLL fractional divider has not enough resolution: %fHz while %fHz is required, try lower frequency or decrease N_ITER in code to achieve more resolution.\n", resolution, symOffset);
       exit(0);
     }
     fracs[i] = freq_corr/delta;
     //printf("i=%u f=%f fa=%f corr=%f delta=%f percfrac=%f int=%u frac=%u tuning_word=%u resolution=%fmHz\n", i, freq, actual_freq, freq_corr, delta, fracs[i], integer_part, fractional_part, tuning_word, resolution *1000);
   }
}

void setupDMA(){
   atexit(unSetupDMA);
   signal (SIGINT, handSig);
   signal (SIGTERM, handSig);
   signal (SIGHUP, handSig);
   signal (SIGQUIT, handSig);

   // allocate a few pages of ram
   getRealMemPage(&constPage.v, &constPage.p);
 
   int instrCnt = 0;
  
   while (instrCnt<1024) {
     getRealMemPage(&instrPage.v, &instrPage.p);
    
     // make copy instructions
     struct CB* instr0= (struct CB*)instrPage.v;
     int i; 
     for (i=0; i<4096/sizeof(struct CB); i++) {
       instrs[instrCnt].v = (void*)((int)instrPage.v + sizeof(struct CB)*i);
       instrs[instrCnt].p = (void*)((int)instrPage.p + sizeof(struct CB)*i);
       instr0->SOURCE_AD = (unsigned int)constPage.p+2048;
       instr0->DEST_AD = PWMBASE+0x18 /* FIF1 */;
       instr0->TXFR_LEN = 4;
       instr0->STRIDE = 0;
       //instr0->NEXTCONBK = (int)instrPage.p + sizeof(struct CB)*(i+1);
       instr0->TI = (1/* DREQ  */<<6) | (5 /* PWM */<<16) |  (1<<26/* no wide*/) ;
       instr0->RES1 = 0;
       instr0->RES2 = 0;

       if (i%2) {
         instr0->DEST_AD = CM_GP0DIV;
         instr0->STRIDE = 4;
         instr0->TI = (1<<26/* no wide*/) ;
       }

       if (instrCnt!=0) ((struct CB*)(instrs[instrCnt-1].v))->NEXTCONBK = (int)instrs[instrCnt].p;
       instr0++;
       instrCnt++;
     }
   }
   ((struct CB*)(instrs[1023].v))->NEXTCONBK = (int)instrs[0].p;

   // set up a clock for the PWM
   ACCESS(CLKBASE + 40*4 /*PWMCLK_CNTL*/) = 0x5A000026;  // Source=PLLD and disable
   usleep(1000);
//   ACCESS(CLKBASE + 41*4 /*PWMCLK_DIV*/)  = 0x5A002800;
   ACCESS(CLKBASE + 41*4 /*PWMCLK_DIV*/)  = 0x5A002000;  // set PWM div to 2, for 250MHz 
   ACCESS(CLKBASE + 40*4 /*PWMCLK_CNTL*/) = 0x5A000016;  // Source=PLLD and enable 
   usleep(1000);

   // set up pwm
   ACCESS(PWMBASE + 0x0 /* CTRL*/) = 0;
   usleep(1000);
   ACCESS(PWMBASE + 0x4 /* status*/) = -1;  // clear errors
   usleep(1000);
   ACCESS(PWMBASE + 0x0 /* CTRL*/) = -1; //(1<<13 /* Use fifo */) | (1<<10 /* repeat */) | (1<<9 /* serializer */) | (1<<8 /* enable ch */) ;
   usleep(1000);
   ACCESS(PWMBASE + 0x8 /* DMAC*/) = (1<<31 /* DMA enable */) | 0x0707;

   //activate dma
   struct DMAregs* DMA0 = (struct DMAregs*)&(ACCESS(DMABASE));
   DMA0->CS =1<<31;  // reset
   DMA0->CONBLK_AD=0;
   DMA0->TI=0;
   DMA0->CONBLK_AD = (unsigned int)(instrPage.p);
   DMA0->CS =(1<<0)|(255 <<16);  // enable bit = 0, clear end flag = 1, prio=19-16
}


//
// Set up a memory regions to access GPIO
//
void setup_io()
{
    /* open /dev/mem */
    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
        printf("can't open /dev/mem \n");
        exit (-1);
    }

    /* mmap GPIO */

    // Allocate MAP block
    if ((gpio_mem = malloc(BLOCK_SIZE + (PAGE_SIZE-1))) == NULL) {
        printf("allocation error \n");
        exit (-1);
    }

    // Make sure pointer is on 4K boundary
    if ((unsigned long)gpio_mem % PAGE_SIZE)
        gpio_mem += PAGE_SIZE - ((unsigned long)gpio_mem % PAGE_SIZE);

    // Now map it
    gpio_map = (unsigned char *)mmap(
                   gpio_mem,
                   BLOCK_SIZE,
                   PROT_READ|PROT_WRITE,
                   MAP_SHARED|MAP_FIXED,
                   mem_fd,
                   GPIO_BASE
               );

    if ((long)gpio_map < 0) {
        printf("mmap error %d\n", (int)gpio_map);
        exit (-1);
    }

    // Always use volatile pointer!
    gpio = (volatile unsigned *)gpio_map;


}

void setup_gpios()
{
   int g;
   // Switch GPIO 7..11 to output mode

    /************************************************************************\
     * You are about to change the GPIO settings of your computer.          *
     * Mess this up and it will stop working!                               *
     * It might be a good idea to 'sync' before running this program        *
     * so at least you still have your code changes written to the SD-card! *
    \************************************************************************/

    // Set GPIO pins 7-11 to output
    for (g=7; g<=11; g++) {
        INP_GPIO(g); // must use INP_GPIO before we can use OUT_GPIO
        //OUT_GPIO(g);
    }

}

void strupr(char *str) 
{   while(*str) 
    { 
        *str = toupper(*str); 
        str++; 
    }
}

void wspr(char* call, char* l, char* dbm, unsigned char* symbols)
{
   // pack prefix in nadd, call in n1, grid, dbm in n2 
   char* c, buf[16];
   strncpy(buf, call, 16);
   c=buf;
   strupr(c);
   unsigned long ng,nadd=0;

   if(strchr(c, '/')){ //prefix-suffix
     nadd=2;
     int i=strchr(c, '/')-c; //stroke position 
     int n=strlen(c)-i-1; //suffix len, prefix-call len
     c[i]='\0';
     if(n==1) ng=60000-32768+(c[i+1]>='0'&&c[i+1]<='9'?c[i+1]-'0':c[i+1]==' '?38:c[i+1]-'A'+10); // suffix /A to /Z, /0 to /9
     if(n==2) ng=60000+26+10*(c[i+1]-'0')+(c[i+2]-'0'); // suffix /10 to /99
     if(n>2){ // prefix EA8/, right align
       ng=(i<3?36:c[i-3]>='0'&&c[i-3]<='9'?c[i-3]-'0':c[i-3]-'A'+10);
       ng=37*ng+(i<2?36:c[i-2]>='0'&&c[i-2]<='9'?c[i-2]-'0':c[i-2]-'A'+10);
       ng=37*ng+(i<1?36:c[i-1]>='0'&&c[i-1]<='9'?c[i-1]-'0':c[i-1]-'A'+10);
       if(ng<32768) nadd=1; else ng=ng-32768;
       c=c+i+1;
     }
   }

   int i=(isdigit(c[2])?2:isdigit(c[1])?1:0); //last prefix digit of de-suffixed/de-prefixed callsign
   int n=strlen(c)-i-1; //2nd part of call len
   unsigned long n1;
   n1=(i<2?36:c[i-2]>='0'&&c[i-2]<='9'?c[i-2]-'0':c[i-2]-'A'+10);
   n1=36*n1+(i<1?36:c[i-1]>='0'&&c[i-1]<='9'?c[i-1]-'0':c[i-1]-'A'+10);
   n1=10*n1+c[i]-'0';
   n1=27*n1+(n<1?26:c[i+1]-'A');
   n1=27*n1+(n<2?26:c[i+2]-'A');
   n1=27*n1+(n<3?26:c[i+3]-'A');
  
   //if(rand() % 2) nadd=0;
   if(!nadd){ 
     strupr(l); //grid square Maidenhead locator (uppercase)
     ng=180*(179-10*(l[0]-'A')-(l[2]-'0'))+10*(l[1]-'A')+(l[3]-'0');
   }
   int p = atoi(dbm);    //EIRP in dBm={0,3,7,10,13,17,20,23,27,30,33,37,40,43,47,50,53,57,60}
   int corr[]={0,-1,1,0,-1,2,1,0,-1,1};
   p=p>60?60:p<0?0:p+corr[p%10];
   unsigned long n2=(ng<<7)|(p+64+nadd);

   // pack n1,n2,zero-tail into 50 bits
   char packed[11] = {n1>>20, n1>>12, n1>>4, ((n1&0x0f)<<4)|((n2>>18)&0x0f), 
n2>>10, n2>>2, (n2&0x03)<<6, 0, 0, 0, 0};

   // convolutional encoding K=32, r=1/2, Layland-Lushbaugh polynomials
   int k = 0;
   int j,s;
   int nstate = 0;
   unsigned char symbol[176];
   for(j=0;j!=sizeof(packed);j++){
      for(i=7;i>=0;i--){
         unsigned long poly[2] = { 0xf2d05351L, 0xe4613c47L };
         nstate = (nstate<<1) | ((packed[j]>>i)&1);
         for(s=0;s!=2;s++){   //convolve
            unsigned long n = nstate & poly[s];
            int even = 0;  // even := parity(n)
            while(n){
               even = 1 - even;
               n = n & (n - 1);
            }
            symbol[k] = even;
            k++;
         }
      }
   }

   // interleave symbols
   const unsigned char npr3[162] = {
      1,1,0,0,0,0,0,0,1,0,0,0,1,1,1,0,0,0,1,0,0,1,0,1,1,1,1,0,0,0,0,0,
      0,0,1,0,0,1,0,1,0,0,0,0,0,0,1,0,1,1,0,0,1,1,0,1,0,0,0,1,1,0,1,0,
      0,0,0,1,1,0,1,0,1,0,1,0,1,0,0,1,0,0,1,0,1,1,0,0,0,1,1,0,1,0,1,0,
      0,0,1,0,0,0,0,0,1,0,0,1,0,0,1,1,1,0,1,1,0,0,1,1,0,1,0,0,0,1,1,1,
      0,0,0,0,0,1,0,1,0,0,1,1,0,0,0,0,0,0,0,1,1,0,1,0,1,1,0,0,0,1,1,0,
      0,0 };
   for(i=0;i!=162;i++){
      // j0 := bit reversed_values_smaller_than_161[i]
      unsigned char j0;
      p=-1;
      for(k=0;p!=i;k++){
         for(j=0;j!=8;j++)   // j0:=bit_reverse(k)
           j0 = ((k>>j)&1)|(j0<<1);
         if(j0<162)
           p++;
      }
      symbols[j0]=npr3[j0]|symbol[i]<<1; //interleave and add sync vector
   }
}

void wait_every(int minute)
{
  time_t t;
  struct tm* ptm;
  for(;;){
    time(&t); 
    ptm = gmtime(&t);
    if((ptm->tm_min % minute) == 0 && ptm->tm_sec == 0) break;
    usleep(1000);
  }
  usleep(1000000); // wait another second
}

int main(int argc, char *argv[])
{
  unsigned char symbols[162];
  int i;
  double centre_freq;
  int wspr15;
  double wspr_symtime;
  int nbands = argc - 4;
  int band = 0;

  if(argc < 5){
    printf("Usage: wspr <[prefix/]callsign[/A-Z,/0-9,/00-99]> <locator> <power in dBm> [<frequency in Hz or 0 for interval> ...]\n");
    printf("\te.g.: sudo ./wspr K1JT/P JO21 10 7040074 0 0 10140174 0 0\n");
    printf("\tchoose freq in range +/-100 Hz around one of center frequencies: 137500, 475700, 1838100, 3594100, 5288700, 7040100, 10140200, 14097100, 18106100, 21096100, 24926100, 28126100, 50294500, 70092500, 144490500 Hz (WSPR-2), or in range +/-12 Hz around 137612, 475812, 1838212 Hz (WSPR-15).\n");
    return 1;
  }
  // argv[1]=callsign, argv[2]=locator, argv[3]=power(dBm)
  wspr(argv[1], argv[2], argv[3], symbols);
  printf("Symbols: ");
  for (i = 0; i < sizeof(symbols)/sizeof(*symbols); i++)
    printf("%d,", symbols[i]);
  printf("\n");
  setup_io();
  setup_gpios();
  txon();
  setupDMA();
  printf("Ready for transmit...\n");

  for(;;)
  {
    txoff();
    centre_freq = atof(argv[band + 4]);
    wspr15 = (centre_freq > 137600 && centre_freq < 137625) || \
             (centre_freq > 475800 && centre_freq < 475825) || \
             (centre_freq > 1838200 && centre_freq < 1838225);
    wspr_symtime = (wspr15) ? 8.0 * WSPR_SYMTIME : WSPR_SYMTIME;
    band++;
    if(band >= nbands)
      band = 0;
    if(centre_freq) setupDMATab(centre_freq, 1.0/wspr_symtime, wspr_symtime, 4);
    wait_every((wspr15) ? 15 : 2);
    time_t t;
    time(&t);
    char buf[256];
    strcpy(buf,ctime(&t));
    buf[strlen(buf)-1]='\0';
    printf("%s - %s@%f\n", buf, (wspr15)?"wspr-15":"wspr-2", centre_freq);
    if(centre_freq){
      txon();
      for (i = 0; i < 162; i++) {
        txSym(symbols[i], wspr_symtime);
        //txSym(atoi(argv[5]), wspr_symtime);
      } 
    }
  }
  return 0;
}

