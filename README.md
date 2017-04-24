 Raspberry Pi bareback LF/MF/HF/VHF WSPR transmitter  <pe1nnz@amsat.org>

 Makes a very simple WSPR beacon from your RasberryPi by connecting GPIO
 port to Antanna (and LPF), operates on LF, MF, HF and VHF bands from
 0 to 250 MHz.

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
  frequency. Do not overclock your RPi as it may make the clock unreliable due to 
  a dynamic clocking feature.

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
  gcc -lm -std=c99 wspr.c -owspr

Issues:
  Two users were reporting that the program never stops transmitting, even
  when intervals for disabled tx are programmed. The problem was in both
  cases fixed by flashing a new image on the SD card with a freshly downloaded 
  image: 2013-02-09-wheezy-raspbian.zip. No apt-get upgrade or firmware
  upgrade was performed. After this WsprryPi TX was running successfully. 

  One user reported his RPi died while in WsprryPi service caused by excessive 
  RF voltage (90V) on GPIO4 created by a 100 watts AM transmitter 50ft away from 
  the antenna. After the damage exessive current was consumed by RPi (1.1A from 
  5V supply), caused by short-circuiting in the 3.3V logic of the BCM2835 SOC.
  On his replacement RPi, he is planning to add galvanic isolation and buffering.


