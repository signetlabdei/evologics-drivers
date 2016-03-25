//
// Copyright (c) 2016 Regents of the SIGNET lab, University of Padova.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the University of Padova (SIGNET lab) nor the 
//    names of its contributors may be used to endorse or promote products 
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
 * @file   GPIO.h
 * @author Filippo Campagnaro
 * @version 1.0.0
 *
 * \brief Implementation of control functions for padnaboard's GPIO
 *
 * Notes:
 * This code has been inspired from Hesham Elsaghir's code available
 * in https://docs.google.com/file/d/0B9Hq_0m0iG6DeDlwT3ZmYUhGYWc/edit
 **/

#include "GPIO.h"

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
	if(! gpio_direction.compare(IN) * gpio_direction.compare(OUT))
	{
		printf("Error: %s is a wrong direction. Please, type %s or %s \n", 
						gpio_direction.c_str(), IN, OUT);
		exit(1);
	}
	//Set pointer to begining of the file
	rewind(fp);
	//Write our value of direction to the file
	strcpy(set_value, gpio_direction.c_str());
	fwrite(&set_value, sizeof(char), gpio_direction.length(), fp);
	fclose(fp);
	printf("...direction set to %s\n", gpio_direction.c_str());
	
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
	if (fread(&get_value, sizeof(char), 1, fp) != ftell(fp)) 
	{
	  fputs ("Reading error",stderr); 
	  exit (1);
	}
	
	printf("Read value %s \n", get_value);
	fclose(fp);
	return new char(*get_value);
}
