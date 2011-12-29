//DS1337.cpp
extern "C" {
	#include <Wire/Wire.h>
	#include <avr/pgmspace.h>
	#include <HardwareSerial.h>
}

#include "programStrings.h"
#include "ds1337.h"
#include "rtcConfig.h"

DS1337::DS1337()
{
#ifdef WIRE_LIB_SCAN_MOD
	clockExists = false;
#endif
	Wire.begin();
}

DS1337 RTC = DS1337();

#ifdef WIRE_LIB_SCAN_MOD
int8_t DS1337::clockInit(void)
{
	// Account for the crystal power up!
	delay(250);

	// Check address and returns false is there is an error
	if (Wire.checkAddress(DS1337_WADDR)) {
		// Possibly set the default registers here

		clockExists	= true;

		// Start the oscillator if need
		if (getRegisterBit(DS1337_SP, DS1337_SP_EOSC))
		{
			clockStart();
		}

		return DS1337_WADDR;
	} else clockExists	= false;

	return -1;
}
#endif

void DS1337::setRegister(uint8_t registerNumber, uint8_t registerMask)
{
	writeRegister(registerNumber, (getRegister(registerNumber) | registerMask));
}

void DS1337::unsetRegister(uint8_t registerNumber, uint8_t registerMask)
{
	writeRegister(registerNumber, (getRegister(registerNumber) & ~registerMask));
}

void DS1337::writeRegister(uint8_t registerNumber, uint8_t registerValue)
{
	Wire.beginTransmission(DS1337_WADDR);
	Wire.send(registerNumber);

	Wire.send(registerValue);

	Wire.endTransmission();
}

uint8_t DS1337::getRegister(uint8_t registerNumber)
{
	Wire.beginTransmission(DS1337_WADDR);
	Wire.send(registerNumber);
	Wire.endTransmission();

	Wire.requestFrom(DS1337_WADDR, 1);

	return Wire.receive();
}

// PRIVATE FUNCTIONS
void DS1337::clockRead(void)
{
	Wire.beginTransmission(DS1337_WADDR);
	Wire.send(0x00);
	Wire.endTransmission();

	Wire.requestFrom(DS1337_WADDR, 7);
	for(int i=0; i<7; i++)
	{
		if (Wire.available())
			rtc_bcd[i]	= Wire.receive();
	}
}

void DS1337::clockSave(void)
{
	Wire.beginTransmission(DS1337_WADDR);
	Wire.send(0x00);

	for(int i=0; i<7; i++)
	{
		Wire.send(rtc_bcd[i]);
	}

	Wire.endTransmission();
}

// PUBLIC FUNCTIONS
void DS1337::clockGet(uint16_t *rtc)
{
	clockRead();

	for(int i=0;i<8;i++)  // cycle through each component, create array of data
	{
		rtc[i]=clockGet(i, 0);
	}
}

uint16_t DS1337::clockGet(uint8_t c, boolean refresh)
{
	if(refresh) clockRead();

	int timeValue=-1;
	switch(c)
	{
		case DS1337_SEC:
			timeValue = (10*((rtc_bcd[DS1337_SEC] & DS1337_HI_SEC)>>4))+(rtc_bcd[DS1337_SEC] & DS1337_LO_BCD);
		break;
		case DS1337_MIN:
			timeValue = (10*((rtc_bcd[DS1337_MIN] & DS1337_HI_MIN)>>4))+(rtc_bcd[DS1337_MIN] & DS1337_LO_BCD);
		break;
		case DS1337_HR:
			timeValue = (10*((rtc_bcd[DS1337_HR] & DS1337_HI_HR)>>4))+(rtc_bcd[DS1337_HR] & DS1337_LO_BCD);
		break;
		case DS1337_DOW:
			timeValue = rtc_bcd[DS1337_DOW] & DS1337_LO_DOW;
		break;
		case DS1337_DATE:
			timeValue = (10*((rtc_bcd[DS1337_DATE] & DS1337_HI_DATE)>>4))+(rtc_bcd[DS1337_DATE] & DS1337_LO_BCD);
		break;
		case DS1337_MTH:
			timeValue = (10*((rtc_bcd[DS1337_MTH] & DS1337_HI_MTH)>>4))+(rtc_bcd[DS1337_MTH] & DS1337_LO_BCD) & ~DS1337_LO_CNTY;
		break;
		case DS1337_YR:
			timeValue = (10*((rtc_bcd[DS1337_YR] & DS1337_HI_YR)>>4))+(rtc_bcd[DS1337_YR] & DS1337_LO_BCD)+(1900 + (rtc_bcd[DS1337_MTH] & DS1337_LO_CNTY ? 100 : 0));
		break;

		case DS1337_CNTY:
			timeValue = rtc_bcd[DS1337_MTH] & DS1337_LO_CNTY>>7;
		break;
	} // end switch

	return timeValue;
}

void DS1337::clockSet(uint8_t timeSection, uint16_t timeValue)
{
	switch(timeSection)
	{
		case DS1337_SEC:
		if(timeValue<60 && timeValue>-1)
		{
			rtc_bcd[DS1337_SEC]		= binToBcd(timeValue);
		}
		break;

		case DS1337_MIN:
		if(timeValue<60 && timeValue>-1)
		{
			rtc_bcd[DS1337_MIN]		= binToBcd(timeValue);
		}
		break;

		case DS1337_HR:
		// TODO : AM/PM  12HR/24HR
		if(timeValue<24 && timeValue>-1)
		{
			rtc_bcd[DS1337_HR]		= binToBcd(timeValue);
		}
		break;

		case DS1337_DOW:
		if(timeValue<8 && timeValue>-1)
		{
			rtc_bcd[DS1337_DOW]		= timeValue;
		}
		break;

		case DS1337_DATE:
		if(timeValue<31 && timeValue>-1)
		{
			rtc_bcd[DS1337_DATE]	= binToBcd(timeValue);
		}
		break;

		case DS1337_MTH:
		if(timeValue<13 && timeValue>-1)
		{
			rtc_bcd[DS1337_MTH]		= (binToBcd(timeValue) & ~DS1337_LO_CNTY) | (rtc_bcd[DS1337_MTH] & DS1337_LO_CNTY);
		}
		break;

		case DS1337_YR:
		if(timeValue<1000 && timeValue>-1)
		{
			rtc_bcd[DS1337_YR]		= binToBcd(timeValue);
		}
		break;

		case DS1337_CNTY:
		if (timeValue > 0)
		{
			rtc_bcd[DS1337_MTH]	= (rtc_bcd[DS1337_MTH] | DS1337_LO_CNTY);
		} else {
			rtc_bcd[DS1337_MTH]	= (rtc_bcd[DS1337_MTH] & ~DS1337_LO_CNTY);
		}
		break;
	} // end switch

	clockSave();
}

uint32_t DS1337::calculateUTS(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
	/* Number of days per month */
	uint32_t tt;
	const uint16_t monthcount[] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

	/* Compute days */
	tt = (year - 1970) * 365 + monthcount[month] + day - 1;

	/* Compute for leap year */
	for (month <= 2 ? year-- : 0; year >= 1970; year--)
		if (isleap(year))
			tt++;

	/* Plus the time */
	tt = sec + 60 * (min + 60 * (tt * 24 + hour - RTC_GMT_OFFSET));

	return tt;
}

void DS1337::clockSetWithUTS(uint32_t unixTimeStamp, boolean correctedTime)
{
	uint16_t	leapCorrection = 0;
	uint32_t	tt;
	uint8_t		thisDate;
	int			ii;
#if defined(RTC_DST_TYPE)
	uint16_t	thisYear;
#endif
	uint16_t	year;
	const uint16_t	monthcount[] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

	/**
	 * Calculate GMT and DST
	**/
#if defined(RTC_GMT_OFFSET)
	if (!correctedTime) unixTimeStamp = unixTimeStamp + (RTC_GMT_OFFSET * 3600);
#endif

	// Years
	tt			= unixTimeStamp / 3600 / 24 / 365;
	year		= tt + 1970;

#if defined(RTC_DST_TYPE)
	if (!correctedTime) {
		thisYear	= year;
	}
#endif

	// Set the century bit
	if (tt > 30) {
		rtc_bcd[DS1337_MTH]	= (rtc_bcd[DS1337_MTH] | DS1337_LO_CNTY);
		tt-= 30;
	} else {
		rtc_bcd[DS1337_MTH]	= (rtc_bcd[DS1337_MTH] & ~DS1337_LO_CNTY);
	}

	// Set the year
	rtc_bcd[DS1337_YR]		= binToBcd(tt);

	// Number of days left in the year
	tt = (unixTimeStamp%31536000 / 3600 / 24) + 1;

	// leap year correction
	for (year--; year > 1970; year--) {
		if (isleap(year))
		{
			leapCorrection++;
			tt--;
		}
	}

	// Set the month
	for (ii = 1; ii < 12; ii++)
	{
		if (monthcount[ii+1] > (tt + ((ii == 2 && isleap(thisYear)) * 1)))
		{
			rtc_bcd[DS1337_MTH]		= (binToBcd(ii) & ~DS1337_LO_CNTY) | (rtc_bcd[DS1337_MTH] & DS1337_LO_CNTY);
			break;
		}
	}

	// Date
#if defined(RTC_DST_TYPE)
	if (!correctedTime) {
		thisDate = tt - monthcount[ii];
	}
#endif

	rtc_bcd[DS1337_DATE]	= binToBcd(tt - monthcount[ii]);

	// Day of the week
	rtc_bcd[DS1337_DOW]		= ((tt)%7 + 1) & DS1337_LO_DOW;

	// Hour
	tt = unixTimeStamp%86400 / 3600;
	rtc_bcd[DS1337_HR]		= binToBcd(tt);

#if defined(RTC_DST_TYPE)
	if (!correctedTime) {
		uint8_t dstStartMo, dstStopMo, dstStart, dstStop;

	#ifndef RTC_CHECK_OLD_DST
		dstStart	=  (31-((thisYear * 5 / 4) + 1) % 7);
	#if RTC_DST_TYPE == 1
	 	dstStop		=  (31-((thisYear * 5 / 4) + 1) % 7);	// EU DST
	#else
		dstStop		= 7 - ((1 + thisYear * 5 / 4) % 7);		// US DST
	#endif
		dstStartMo	= 3;
		dstStopMo	= 11;
	#else
		if (thisYear < 2006) {
			dstStart	= (2+6 * thisYear - (thisYear / 4) ) % 7 + 1;
			dstStop		= 14 - ((1 + thisYear * 5 / 4) % 7);
			dstStartMo	= 4;
			dstStopMo	= 10;
		} else {
			dstStart	=  (31-((thisYear * 5 / 4) + 1) % 7);
		#if RTC_DST_TYPE == 1
		 	dstStop		=  (31-((thisYear * 5 / 4) + 1) % 7);	// EU DST
		#else
			dstStop		= 7 - ((1 + thisYear * 5 / 4) % 7);		// US DST
		#endif
			dstStartMo	= 3;
			dstStopMo	= 11;
		}
	#endif
		if (ii >= dstStartMo && ii <= dstStopMo)
		{
			if (ii < dstStopMo)
			{
				if (ii > dstStartMo || thisDate > dstStart || thisDate == dstStart && tt >= 2)
				{
					clockSetWithUTS(unixTimeStamp + 3600, true);
					return;
				}
			} else {
				if (thisDate < dstStop || thisDate == dstStop && tt < 2)
				{
					clockSetWithUTS(unixTimeStamp + 3600, true);
					return;
				}
			}
		}
	}
#endif

	// Minutes
	tt = unixTimeStamp%3600 / 60;
	rtc_bcd[DS1337_MIN]		= binToBcd(tt);

	// Seconds
	tt = (unixTimeStamp%3600)%60;
	rtc_bcd[DS1337_SEC]		= binToBcd(tt);

	// Stop the clock
	//clockStop();

	// Save buffer to the RTC
	clockSave();

	// Restart the oscillator
	//clockStart();
}

void DS1337::printRegisters(void)
{
	for(int ii=0;ii<0x10;ii++)
	{
		SPrint("0x");
		Serial.print(ii, HEX);
		SPrint(" ");
		Serial.println(getRegister(ii), BIN);
	}

	delay(200);
}