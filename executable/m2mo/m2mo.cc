#include "utilities.h"
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

static char const* check_tx_log_ = "MacTx.txt";
static char const* tx_log_ = "FromMac2Phy.txt";

static char const* check_rx_log_ = "PhyRx";
//static char const* rx_log_ = "'PhyRx*.txt";
static char const* rx_log_ = "FromPhy2Mac";
static char const* extension_ = ".txt";

static char const token_separator_ = ',';
//DONE: check if the transmitting file exists
bool checkTxLog() {
	ifstream input_file_;
	input_file_.open(check_tx_log_);
	bool transmitting_flag = input_file_.is_open();
	if (!transmitting_flag) { input_file_.close(); }
	return transmitting_flag;
}

//DONE: retreive sending time and data to send
std::string readMessage() {
	std::string message_line = "";
	if (!checkTxLog())
		return message_line;
	ifstream input_file_;
	input_file_.open(tx_log_);
	if ( !input_file_.is_open() ) 
		return message_line;
	std::getline(input_file_, message_line);
	remove(check_tx_log_);
	remove(tx_log_);
	return message_line;
}

double tod() {
	timeval tv;
	gettimeofday(&tv, 0);
	double s = tv.tv_sec;
	s += (1e-6 * tv.tv_usec);
	return s;
}

int time2epoch(int hour, int min, int sec) {
	time_t rawtime;
	time (&rawtime);
	struct tm * timeinfo = localtime (&rawtime);
	timeinfo -> tm_sec = sec;
	timeinfo -> tm_min = min;
	timeinfo -> tm_hour = hour;
	return mktime(timeinfo);
}

//DONE: retreive how long we need to wait before to tx the message, parsing the
//      line imported from the TxLog file and subtracting the current time

long parseWait(std::string message2parse) {
	short hour = 0;
	short min = 0;
	short sec = 0;
	::std::stringstream line_stream(message2parse);
	std::string result_ = "";
	std::getline(line_stream, result_, token_separator_);
	hour = atoi(result_.c_str());
	std::getline(line_stream, result_, token_separator_);
	min = atoi(result_.c_str());
	std::getline(line_stream, result_, token_separator_);
	sec = atoi(result_.c_str());

	return 1e6 * (time2epoch(hour,min,sec) - tod());
}

//DONE: retreive the message tx, parsing the line imported from the TxLog file
std::string getMessage(std::string message2parse) {
	std::string message = "";
	::std::stringstream line_stream(message2parse);
	for (int i = 0; i < 3 && std::getline(line_stream, message, token_separator_); i++);
	std::getline(line_stream, message, token_separator_);
	return message;
}

//DONE: print in the received log file and create the flag file for the reception
void printToFile(std::string rx_msg, int counter) { 
	char log_name [100];
	sprintf(log_name, "%s%d%s", rx_log_, counter, extension_);
	std::ofstream out_file_stats;
	out_file_stats.open(log_name, std::ofstream::out); //erase and create a new file
	out_file_stats << rx_msg << std::endl;
	out_file_stats.close();
	sprintf(log_name, "%s%d%s", check_rx_log_, counter, extension_);
	out_file_stats.open(log_name, std::ofstream::out); //erase and create a new file
	out_file_stats.close();
}

int main(int argc, char* argv[]) {
	cout << "Ciao" << endl;
	if (argc != 4) {
		cerr << "Usage:" <<endl;
    cerr << "./mycc ID ip port" << endl;
		return -1;
	}
	int ID = atoi(argv[1]);
	std::string ip = argv[2];
	std::string port = argv[3];
	int RECEIVER = 255; // broadcast
	int rx_counter(0);
	cout << "ID " << ID << endl;
	MdriverS2C_EvoLogics* pmDriver = connectModem(ip, port, ID);
	int modemStatus_old = pmDriver->getStatus();
	int modemStatus = pmDriver->updateStatus();
	const int min_sleeping_time = 200; // 200 ms 
	//main cicle

	while (true) {
	//check if there's something to tx
		bool tx_flag = checkTxLog();
		//cout << "flag = " << tx_flag << endl;
		if (tx_flag) {
			//retreive sending time and data to send
			std::string message2parse = readMessage();
			cout << "message2parse = " << message2parse << endl;
			cout << "getMessage(message2parse)" << getMessage(message2parse) <<endl;
			cout << "parseWait(message2parse) = " << parseWait(message2parse) << endl;
			usleep (parseWait(message2parse)); //sleep(); if sec.
			transmit(pmDriver,RECEIVER, getMessage(message2parse),NOACK);
			std::cout << "Sent " << ID << " to " << RECEIVER << " " << getMessage(message2parse) << endl;
		}
		else {
			//check if there is something received and write it in the log
			if (modemStatus == MODEM_IDLE_RX && modemStatus_old == MODEM_RX) {
				std::string rx_msg = pmDriver->getRxPayload();
				rx_counter ++;
				printToFile(rx_msg,rx_counter);
				cout << "Rx Message " << ID << " " << rx_msg << endl;
				pmDriver -> resetModemStatus();
			}
			modemStatus_old = pmDriver->getStatus();
			modemStatus = pmDriver->updateStatus();

		}
		usleep(min_sleeping_time);

	}
	return 0;
}


