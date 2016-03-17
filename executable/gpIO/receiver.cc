#include "utilities.h"
#include "GPIO.h"

// Global Declaration:
//GPIO setting
int GPIO_status = 0;
const int LOW = 0;
const int HIGH = 1;
const std::string GPIO_RED_pin = "32";
const std::string GPIO_GREEN_pin = "33";
const std::string GPIO_direction= "out";

const std::string ALLARM_Message= "1";

int main(int argc, char* argv[]) {
	cout << "Ciao Stazione" << endl;
	if (argc < 5) {
		cerr << "Insert ID, RECEIVER, ip and port!" << endl;
		return -1;
	}
	//set connection to the acoustic modem
	int ID = atoi(argv[1]);
	int RECEIVER = atoi(argv[2]);
	std::string ip = argv[3];
	std::string port = argv[4];
	cout << "ID " << ID << endl;
	MdriverS2C_EvoLogics* pmDriver = connectModem(ip, port, ID);
	int modemStatus_old = pmDriver->getStatus();
	int modemStatus = pmDriver->updateStatus();

	//set connection to pandaboard GPIO
	Init_GPIO(GPIO_RED_pin,GPIO_direction);
	Init_GPIO(GPIO_GREEN_pin,GPIO_direction);
	std::string led = GPIO_GREEN_pin;
	while (true) {
		if (modemStatus == MODEM_IDLE_RX && modemStatus_old == MODEM_RX) {
			std::string rxMessage = pmDriver->getRxPayload();
			cout << "Ricevuto " << ID << " " << rxMessage << endl;
			if (!(abs(rxMessage.substr(0,1).compare(ALLARM_Message)) + abs(led.compare(GPIO_GREEN_pin)))) {
				toggle_output(led, LOW);
				led = GPIO_RED_pin;
				toggle_output(led, HIGH);
				GPIO_status = HIGH;
				cout << "ALLARM" << endl;
			}
			else if (!(abs(led.compare(GPIO_RED_pin)) + !(rxMessage.substr(0,1).compare(ALLARM_Message)))) {
				toggle_output(led, LOW);
				led = GPIO_GREEN_pin;
				toggle_output(led, HIGH);
				GPIO_status = HIGH;
			}
		}
		toggle_output(led, GPIO_status);
		GPIO_status = ! GPIO_status;
		sleep(1);			
		modemStatus_old = pmDriver->getStatus();
		modemStatus = pmDriver->updateStatus();
	}
	return 0;
}
