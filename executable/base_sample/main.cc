#include "utilities.h"
/*std::string create_message(std::string m, int c, int size) {
	std::stringstream ss;
	ss << m;
        ss << c;
	if (ss.str().size()> size) {
		cerr << "Message too long" << endl;
	}
	while (ss.str().size() < size) 
		ss << " ";
	return ss.str();
}*/

int main(int argc, char* argv[]) {
	cout << "Ciao" << endl;
	if (argc < 8) {
		cerr << "Usage:" <<endl;
                cerr << "./mycc ID ID_SENDER ID_RECEIVER ip port period[ms] packet_in_a_burst" << endl;
		return -1;
	}
        int ID = atoi(argv[1]);
	int SENDER = atoi(argv[2]);
	int RECEIVER = atoi(argv[3]);
	std::string ip = argv[4];
	std::string port = argv[5];
	double period = atof(argv[6]) * pow10(3);
	int BURST = atoi(argv[7]);
	
	int message_length = 18;
     	int backoff_period = 3 * pow10(6);

	cout << "ID " << ID << endl;
	MdriverS2C_EvoLogics* pmDriver = connectModem(ip, port, ID);
	int modemStatus_old = pmDriver->getStatus();
	int modemStatus = pmDriver->updateStatus();

	if (ID == SENDER) {
		int packet_counter = 1;
		std::string message = "CIAO CIAO";
		while (true) {
			std::string ss = create_message(message,packet_counter,message_length);
			pmDriver->updateTx(RECEIVER, ss);
			pmDriver->modemTx();
			cout << "Inviato " << ID << " to " << RECEIVER << " " << ss << endl;
			usleep(period);
			packet_counter ++;
			if ( !(packet_counter % BURST )) {
				std::string ss = create_message(message,packet_counter,message_length);
				transmit(pmDriver,RECEIVER, ss,NOACK);
				cout << "Inviato con ack " << ID << " to " << RECEIVER << " " << ss << endl;
				usleep(period);
				packet_counter ++;
			    cout << "Backoff, pacchetto " << packet_counter << " period = " << period << "[usec]" << endl;
				usleep(backoff_period);
				int d = pmDriver->getDelay();
				cout << "Delay = " << d << endl;
			}
		}
	}
	else {
		while (true) {
			if (modemStatus == RX_STATE_IDLE)
				cout << "Ricevuto " << ID << " " << pmDriver->getRxPayload() << endl;
			sleep(3);			
			modemStatus_old = pmDriver->getStatus();
			modemStatus = pmDriver->updateStatus();
		}
	}
	return 0;
}


