#include "controller.h"

#include <libserialport.h>
#include "A4963_config.h"

#include <string.h>
#include <iostream>

#include <unistd.h>

Controller::Controller():
	mSerialPort(NULL), mLastDiagnostic(0), mCurrentDiagnostic(0), mDiagnosticMask(0)
{
	connect();
}

Controller::~Controller()
{
	disconnect();
}

bool Controller::connect()
{
	struct sp_port **ports;
	if(sp_list_ports(&ports)==SP_OK)
	{
		size_t i=0;
		while(!isConnected() && ports[i])
		{
			try_connect(ports[i]);
			++i;
		}
		sp_free_port_list(ports);
	}
	return isConnected();
}

bool Controller::try_connect(sp_port *aPort)
{
	sp_copy_port(aPort, &mSerialPort);
	
	if(sp_open(mSerialPort, SP_MODE_READ_WRITE) != SP_OK)
		return false;
	
	if((sp_set_baudrate(mSerialPort, 115200)==SP_OK) && (sp_set_bits(mSerialPort, 8)==SP_OK) &&
	(sp_set_parity(mSerialPort, SP_PARITY_NONE)==SP_OK) && (sp_set_stopbits(mSerialPort, 1)==SP_OK))
	{
		usleep(2000000);
		if(!ping())
		{
			disconnect();
			return false;
		}
	}
	return true;
}

bool Controller::ping()
{
	if(!isConnected())
		return false;
	char result[6];
	result[5]=0;
	int ret;
	ret=sp_blocking_write(mSerialPort, "p", 1, 1000);
	if((ret=sp_blocking_read(mSerialPort, result, 5, 1000))<5)
	{
		return false;
	}
	return !strcmp(result, "A4963");
}

bool Controller::disconnect()
{
	if(isConnected())
	{
		sp_close(mSerialPort);
		sp_free_port(mSerialPort);
		mSerialPort=NULL;
	}
	return true;
}

bool Controller::isConnected()
{
	return mSerialPort;
}

bool Controller::setConfig(const A4963Config &aConfig, uint8_t aAddress)
{
	if(!isConnected())
		return false;
	
	if(aAddress==ALL_ADDRESS || aAddress==CONFIG0_ADDRESS)
		write(CONFIG0_ADDRESS, aConfig.reg_config0);
	if(aAddress==ALL_ADDRESS || aAddress==CONFIG1_ADDRESS)
		write(CONFIG1_ADDRESS, aConfig.reg_config1);
	if(aAddress==ALL_ADDRESS || aAddress==CONFIG2_ADDRESS)
		write(CONFIG2_ADDRESS, aConfig.reg_config2);
	if(aAddress==ALL_ADDRESS || aAddress==CONFIG3_ADDRESS)
		write(CONFIG3_ADDRESS, aConfig.reg_config3);
	if(aAddress==ALL_ADDRESS || aAddress==CONFIG4_ADDRESS)
		write(CONFIG4_ADDRESS, aConfig.reg_config4);
	if(aAddress==ALL_ADDRESS || aAddress==CONFIG5_ADDRESS)
		write(CONFIG5_ADDRESS, aConfig.reg_config5);
	if(aAddress==ALL_ADDRESS || aAddress==RUN_ADDRESS)
		write(RUN_ADDRESS, aConfig.reg_run);
	if(aAddress==ALL_ADDRESS || aAddress==MASK_ADDRESS)
		write(MASK_ADDRESS, aConfig.reg_mask);
	if(aAddress==ALL_ADDRESS || aAddress==PWM_ADDRESS)
		write(PWM_ADDRESS, aConfig.pwm_value);
	
	mDiagnosticMask=aConfig.reg_mask;
	return true;
}

bool Controller::setPWM(uint8_t aValue)
{
	if(!isConnected())
		return false;
	write(PWM_ADDRESS, aValue);
	return true;
}

bool Controller::write(uint8_t aAddress, uint16_t aValue)
{
	sp_blocking_write(mSerialPort, "w", 1, 1000);
	sp_blocking_write(mSerialPort, &aAddress, 1, 1000);
	sp_blocking_write(mSerialPort, &aValue, 2, 1000);
	uint16_t result;
	sp_blocking_read(mSerialPort, &result, 2, 1000);
	mCurrentDiagnostic=result;
	if(result!=0)
		mLastDiagnostic=result;
	return true;
}

bool Controller::getConfig(A4963Config &aConfig, uint8_t aAddress)
{
	if(!isConnected())
		return false;
	
	if(aAddress==ALL_ADDRESS || aAddress==CONFIG0_ADDRESS)
		read(CONFIG0_ADDRESS, aConfig.reg_config0);
	if(aAddress==ALL_ADDRESS || aAddress==CONFIG1_ADDRESS)
		read(CONFIG1_ADDRESS, aConfig.reg_config1);
	if(aAddress==ALL_ADDRESS || aAddress==CONFIG2_ADDRESS)
		read(CONFIG2_ADDRESS, aConfig.reg_config2);
	if(aAddress==ALL_ADDRESS || aAddress==CONFIG3_ADDRESS)
		read(CONFIG3_ADDRESS, aConfig.reg_config3);
	if(aAddress==ALL_ADDRESS || aAddress==CONFIG4_ADDRESS)
		read(CONFIG4_ADDRESS, aConfig.reg_config4);
	if(aAddress==ALL_ADDRESS || aAddress==CONFIG5_ADDRESS)
		read(CONFIG5_ADDRESS, aConfig.reg_config5);
	if(aAddress==ALL_ADDRESS || aAddress==RUN_ADDRESS)
		read(RUN_ADDRESS, aConfig.reg_run);
	if(aAddress==ALL_ADDRESS || aAddress==MASK_ADDRESS)
		read(MASK_ADDRESS, aConfig.reg_mask);
	
	return true;
}

bool Controller::read(uint8_t aAddress, uint16_t &aValue)
{
	sp_blocking_write(mSerialPort, "r", 1, 1000);
	sp_blocking_write(mSerialPort, &aAddress, 1, 1000);
	sp_blocking_read(mSerialPort, &aValue, 2, 1000);
	return true;
}

bool Controller::getFaultStatus(bool &aFault)
{
	if(!isConnected())
		return false;
	uint16_t result;
	read(FAULT_ADDRESS, result);
	aFault=result;
	return true;
}

bool Controller::getLastDiagnostic(uint16_t &aDiagnostic)
{
	aDiagnostic=mLastDiagnostic;
	return true;
}

bool Controller::getCurrentDiagnostic(uint16_t &aDiagnostic)
{
	if(!isConnected())
		return false;
	write(MASK_ADDRESS, mDiagnosticMask);
	aDiagnostic=mCurrentDiagnostic;
	return true;
}

bool Controller::getSpeed(uint16_t &aSpeed)
{
	if(!isConnected())
		return false;
	read(SPEED_ADDRESS, aSpeed);
	return true;
}



