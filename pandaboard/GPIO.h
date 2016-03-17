/**
 * Hesham Elsaghir
 * First Project to test the PandaBoard programming.
 * May 20th, 2013
 * Notes:
 * You will have to run this program using sudo ./Test_IO_1
 * I still need to see how to add something while booting to create the GPIO and make RW to all users
 * */



#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <iostream>


// Function Declaration:
int Init_GPIO(const std::string gpio_pin, const std::string gpio_direction);
void toggle_output(const std::string gpio_pin , int GPIO_Value);
char* toggle_input(const std::string gpio_pin);

// Global declaration

FILE *fp;

int Init_GPIO(const std::string gpio_pin ,const std::string gpio_direction)
{
	//create a variable to store whether we are sending a '1' or a '0'
	char set_value[4]; 

	printf("Initializing system...\n");
	
	//Using sysfs we need to write "32" to /sys/class/gpio/export
	//This will create the folder /sys/class/gpio/gpioPIN
	if ((fp = fopen("/sys/class/gpio/export", "ab")) == NULL)
		{
			printf("Cannot open export file.\n");
			exit(1);
		}
	//Set pointer to begining of the file
	rewind(fp);
	strcpy(set_value, gpio_pin.c_str());
	fwrite(&set_value, sizeof(char), gpio_pin.length(), fp);
	fclose(fp);
	
	printf("...export file accessed, new pin now accessible\n");
	
	///*********************************************************
	//SET DIRECTION
	//Open the LED's sysfs file in binary for reading and writing, store file pointer in fp
	const char * gpio_direction_path = ("/sys/class/gpio/gpio" + gpio_pin +"/direction").c_str();
	if ((fp = fopen(gpio_direction_path, "rb+")) == NULL)
	{
		printf("Cannot open direction file.\n");
		exit(1);
	}
	//Set pointer to begining of the file
	rewind(fp);
	//Write our value of "out" to the file
	strcpy(set_value, gpio_direction.c_str());
	fwrite(&set_value, sizeof(char), gpio_direction.length(), fp);
	fclose(fp);
	printf("...direction set to output\n");
	
	printf("...init OK...\n");
	
	
	return 0;
}

void toggle_output(const std::string gpio_pin , int GPIO_Value)
{
	char set_value[4];
	const char * gpio_value_path = ("/sys/class/gpio/gpio" + gpio_pin +"/value").c_str();
	if ((fp = fopen(gpio_value_path, "rb+")) == NULL)
	{
		printf("Cannot open value file.\n");
		exit(1);
	}
		
	if(GPIO_Value)
	{
		//Set pointer to begining of the file
		rewind(fp);
		//Write our value of "1" to the file 
		strcpy(set_value,"1");
		fwrite(&set_value, sizeof(char), 1, fp);
		fclose(fp);
		printf("...value set to 1...\n");
	}
	else
	{
		//Set pointer to begining of the file
		rewind(fp);
		//Write our value of "0" to the file 
		strcpy(set_value,"0");
		fwrite(&set_value, sizeof(char), 1, fp);
		fclose(fp);
		printf("...value set to 0...\n");
	}
}

char* toggle_input(const std::string gpio_pin)
{
	char get_value[4] = "   ";
	const char * gpio_value_path = ("/sys/class/gpio/gpio" + gpio_pin +"/value").c_str();
	if ((fp = fopen(gpio_value_path, "rb+")) == NULL)
	{
		printf("Cannot open value file.\n");
		exit(1);
	}
	
	//Set pointer to begining of the file
	rewind(fp);
	//Read from the file 
	fread(&get_value, sizeof(char), 1, fp);
	printf("Read value %s \n", get_value);
	fclose(fp);
	return get_value;
}
