#include "utilities.h"

// Global Declaration:
//GPIO setting
const int LOW = 0;
const int HIGH = 1;
const std::string GPIO_RED_pin = "32";
const std::string GPIO_GREEN_pin = "33";
const std::string GPIO_direction= "out";

const std::string ALLARM_Message= "1";

int main(int argc, char* argv[]) {
	cout << "Ciao Stazione" << endl;
	if (argc < 4) {
		cerr << "Insert ID_RECEIVER, ip and port!" << endl;
		return -1;
	}
	//set connection to the acoustic modem
	int ID_RECEIVER = atoi(argv[1]);
	std::string ip = argv[2];
	std::string port = argv[3];
	cout << "ID " << ID_RECEIVER << endl;
	MdriverS2C_EvoLogics* pmDriver = connectModem(ip, port, ID_RECEIVER);
	int modemStatus_old = pmDriver->getStatus();
	int modemStatus = pmDriver->updateStatus();

	while (true) {
		if (modemStatus == MODEM_IDLE_RX && modemStatus_old == MODEM_RX) {
			std::string rxMessage = pmDriver->getRxPayload();
			cout << "Ricevuto " << ID_RECEIVER << " " << rxMessage << endl;
		}
		usleep(200);			
		modemStatus_old = pmDriver->getStatus();
		modemStatus = pmDriver->updateStatus();
	}
	return 0;
}
