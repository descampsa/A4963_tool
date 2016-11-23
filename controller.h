#pragma once

#include <stdint.h>

#define CONFIG0_ADDRESS 0
#define CONFIG1_ADDRESS 1
#define CONFIG2_ADDRESS 2
#define CONFIG3_ADDRESS 3
#define CONFIG4_ADDRESS 4
#define CONFIG5_ADDRESS 5
#define MASK_ADDRESS 6
#define RUN_ADDRESS 7
#define FAULT_ADDRESS 8
#define SPEED_ADDRESS 9
#define PWM_ADDRESS 10

#define ALL_ADDRESS 11

extern "C" {
struct sp_port;
}
class A4963Config;

class Controller
{
	public:
	Controller();
	~Controller();
	
	bool connect();
	bool disconnect();
	
	bool isConnected();
	
	bool setConfig(const A4963Config &aConfig, uint8_t aAddress=ALL_ADDRESS);
	bool getConfig(A4963Config &aConfig, uint8_t aAddress=ALL_ADDRESS);
	
	bool setPWM(uint8_t aValue);
	bool getFaultStatus(bool &aFault);
	bool getLastDiagnostic(uint16_t &aDiagnostic);
	bool getCurrentDiagnostic(uint16_t &aDiagnostic);
	bool getSpeed(uint16_t &aSpeed);
	
	private:
	
	bool try_connect(sp_port *aPort);
	bool ping();
	
	bool write(uint8_t aAddress, uint16_t aValue);
	bool read(uint8_t aAddress, uint16_t &aValue);
	
	sp_port *mSerialPort;
	uint16_t mLastDiagnostic, mCurrentDiagnostic, mDiagnosticMask;
};

