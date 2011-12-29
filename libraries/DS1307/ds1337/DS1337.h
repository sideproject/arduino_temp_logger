/*
  DS1337.h - library for DS1337 rtc
*/

// ensure this library description is only included once
#ifndef DS1337_h
#define DS1337_h

// include types & constants of Wiring core API
#include <WConstants.h>

// include types & constants of Wire ic2 lib
#include <Wire/Wire.h>

#include "rtcConfig.h"

#define DS1337_SEC	0
#define DS1337_MIN	1
#define DS1337_HR	2
#define DS1337_DOW	3
#define DS1337_DATE 4
#define DS1337_MTH	5
#define DS1337_YR	6
#define DS1337_CNTY	7

#define RTC_SEC		DS1337_SEC
#define RTC_MIN		DS1337_MIN
#define RTC_HR		DS1337_HR
#define RTC_DOW		DS1337_DOW
#define RTC_DATE	DS1337_DATE
#define RTC_MTH		DS1337_MTH
#define RTC_YR		DS1337_YR
#define RTC_CNTY	DS1337_CNTY

#define DS1337_WADDR		0x68
#define DS1337_RADDR		DS1337_WADDR | 0x01

/**
 * Define register bit masks
**/
#define DS1337_LO_BCD		B00001111
#define DS1337_HI_BCD		B01110000

#define DS1337_HI_SEC		B01110000
#define DS1337_HI_MIN		B01110000
#define DS1337_HI_HR		B00110000
#define DS1337_LO_DOW		B00000111
#define DS1337_HI_DATE		B00110000
#define DS1337_HI_MTH		B00010000
#define DS1337_LO_CNTY		B10000000
#define DS1337_HI_YR		B11110000

#define DS1337_ARLM1		0x07
#define DS1337_ARLM1_LO_SEC	B00001111
#define DS1337_ARLM1_HI_SEC	B01110000
#define DS1337_ARLM1_LO_MIN	B01110000
#define DS1337_ARLM1_HI_MIN	B00001111

#define DS1337_SP			0x0E
#define	DS1337_SP_EOSC		B10000000
#define	DS1337_SP_RS2		B00010000
#define	DS1337_SP_RS1		B00001000
#define	DS1337_SP_INTCN		B00000100
#define	DS1337_SP_A2IE		B00000010
#define	DS1337_SP_A1IE		B00000001

#define DS1337_STATUS		0x0F
#define DS1337_STATUS_OSF	B10000000
#define DS1337_STATUS_A2F	B00000010
#define DS1337_STATUS_A1F	B00000001

/**
 * Macros
**/
#define clockStart()					unsetRegister(DS1337_SP, DS1337_SP_EOSC)
#define clockStop()						setRegister(DS1337_SP, DS1337_SP_EOSC)

#define getRegisterSP()					getRegisterSP(DS1337_SP)
#define getRegisterStatus()				getRegisterStatus(DS1337_STATUS)

#define getRegisterBit(reg, bitMask)	getRegister(reg) & bitMask

#define isleap(y)						((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)

#define bcdToBin(val) 					(((val)&15) + ((val)>>4)*10)
#define binToBcd(val) 					((((val)/10)<<4) + (val)%10)

/**
 * getUTS: Macro for calculateUTS
 * returns the time as a unix time stamp
 * This function doesn't take into account having DST set or not!
**/
#define	getUTS(refresh)				calculateUTS(	RTC.clockGet(DS1337_YR, true), RTC.clockGet(DS1337_MTH, false), \
														RTC.clockGet(DS1337_DATE, false), RTC.clockGet(DS1337_HR, false), \
														RTC.clockGet(DS1337_MIN, false), RTC.clockGet(DS1337_SEC, false) \
													)

#define clockSet(UTS)					clockSetWithUTS(UTS, false)

// library interface description
class DS1337
{
	// user-accessible "public" interface
	public:
		/**
		 * clockExists: keeps track of the whether or not the RTC exists
		**/
	#ifdef WIRE_LIB_SCAN_MOD
		bool		clockExists;
	#endif
		/**
		 * Class constructor
		**/
		DS1337();

		/**
		 * clockInit: initializes the clock
		 * If the I2C scan mod is available, it'll verify the RTC is reachable
		**/
		int8_t		clockInit(void);

		/**
		 * setRegister: sets a register bit fromt he register number and bitmask
		**/
		void		setRegister(uint8_t, uint8_t);

		/**
		 * unsetRegister: unsets a register bit fromt he register number and bitmask
		**/
		void		unsetRegister(uint8_t, uint8_t);

		/**
		 * getRegister: returns the specified register
		**/
		uint8_t		getRegister(uint8_t);

		/**
		 * clockGet: fills an array with the current time data
		**/
		void		clockGet(uint16_t *);

		/**
		 * clockGet: gets a specific item from the clock buffer
		 * use the second param to specify a buffer refresh
		**/
		uint16_t	clockGet(uint8_t, boolean);

		/**
		 * calculateUTS: returns the time as a unix time stamp
		 * This function doesn't take into account having DST set or not!
		**/
		uint32_t	calculateUTS(uint16_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

		/**
		 * clockSetWithUTS: sets the date & time from a unix time stamp
		 * pass the second param as true to skip DTS and GMT calculation
		**/
		void		clockSetWithUTS(uint32_t, boolean);

		/**
		 * clockSet: Set the clock time using integer values
		**/
		void		clockSet(uint8_t, uint16_t);

		/**
		 * Prints all of the DS1337 registers
		**/
		void		printRegisters(void);
	private:
		uint8_t		rtc_bcd[8];
		void		writeRegister(uint8_t, uint8_t);
		void		clockSave(void);
		void		clockRead(void);
};

extern DS1337 RTC;

#endif