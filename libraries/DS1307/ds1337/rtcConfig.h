//rtcConfig.h
#ifndef RTC_CONFIG_H
#defin#ifndef RTC_CONFIG_H
#define RTC_CONFIG_H

/**
 * Set this to your local GMT offset
 * 	Comment to skip GMT offset calculations
**/
#define	RTC_GMT_OFFSET	-5

/**
 * Select your DST type
 * 	Comment all to skip DST calculation
**/
#define	RTC_DST_TYPE	0 /* US */
//#define	RTC_DST_TYPE	1 /* EU */

// Uncomment this only if you need to check DST prior to 2006
//#define	RTC_CHECK_OLD_DST

#define RTC_DOW_1	PSTR("Sun")	/* First day of the week */
#define RTC_DOW_2	PSTR("Mon")
#define RTC_DOW_3	PSTR("Tue")
#define RTC_DOW_4	PSTR("Wed")
#define RTC_DOW_5	PSTR("Thu")
#define RTC_DOW_6	PSTR("Fri")
#define RTC_DOW_7	PSTR("Sat")

#define RTC_DOW_0	RTC_DOW_4	/* Wednesday is the first day of Epoch: This probably shouldn't change! */

#endif
 