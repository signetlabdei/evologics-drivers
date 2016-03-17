#include "utilities.h"
#include "GPIO.h"

// Global Declaration:
//GPIO setting
int GPIO_32_status = 0;
const std::string GPIO_32_pin = "32";
const std::string GPIO_32_direction= "in";
//Modem messages
static const std::string MESSAGE_STATUS_OK = "0";
static const std::string MESSAGE_STATUS_ALLARM = "1";
int main(int argc, char* argv[]) {
	//check arguments
	if (argc < 5) {
		cerr << "Insert ID, RECEIVER, ip and port!" << endl;
		return -1;
	}
	//set connection to the acoustic modem
	cout << "Ciao" << endl;
	int ID = atoi(argv[1]);
	int RECEIVER = atoi(argv[2]);
	std::string ip = argv[3];
	std::string port = argv[4];
	cout << "ID " << ID << endl;
	MdriverS2C_EvoLogics* pmDriver = connectModem(ip, port, ID);

	//set connection to pandaboard GPIO
	Init_GPIO(GPIO_32_pin,GPIO_32_direction);
		
	//check sensor and signal
	int ciclo = 1;
	while (true) {
		std::string value = toggle_input(GPIO_32_pin);
		std::string message = (value.substr(0,1).compare("1")==0) ? MESSAGE_STATUS_ALLARM : MESSAGE_STATUS_OK;
		transmit(pmDriver,RECEIVER, message, NOACK);
		cout << "Ciclo" << ciclo << "Inviato " << ID << " to " << RECEIVER << " " << message << endl;
		sleep(1);
		ciclo++;
	}
	return 0;
}
