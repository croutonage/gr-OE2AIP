#ifndef DSTAR_HEADER_DECODER_H
#define DSTAR_HEADER_DECODER_H


	typedef struct {
				
		unsigned char flag1;
		unsigned char flag2;
		unsigned char flag3;
		
		
		bool isData;
		bool viaRepeater;
		bool interruption;
		bool isControlSignal;
		bool emergency;
		
		char destinationRepeater[9];
		char departureRepeater[9];
		char companionCallsign[9];
		char ownCallsign[9];
		char ownCallsignSuffix[5];
		
		unsigned int errors;
		
		unsigned short checksum;
		unsigned short calculatedChecksum;
		
	} header_t;

	void headerDecode(unsigned char raw_header[660], bool softDecition, header_t *header);

#endif
