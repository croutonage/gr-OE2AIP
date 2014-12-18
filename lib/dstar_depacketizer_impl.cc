#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "dstar_depacketizer_impl.h"

#include "dstar_header_decoder.h"
#include "dstar.h"
#include "rp2c.h"

#define FRAME_SYNC 0x55557650
#define SYNC_FLAG 0x00AAB468
#define TERMINATION_FLAG 0xAAAA135E



// Header aus der Simulation von DL3OCK
        char raw_header_demo[660] = {0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0,
                                1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1,
                                1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
                                1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0,
                                0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1,
                                1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0,
                                0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0,
                                1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1,
                                1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0,
                                0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1,
                                1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0,
                                1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1,
                                1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0,
                                1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1,
                                1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0,
                                1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1,
                                0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0,
                                0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0,
                                1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1,
                                1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1};

namespace gr {
  namespace OE2AIP {

    using namespace std;

	

    dstar_depacketizer::sptr
    dstar_depacketizer::make()
    {
      return gnuradio::get_initial_sptr
        (new dstar_depacketizer_impl());
    }

    /*
     * The private constructor
     */
    dstar_depacketizer_impl::dstar_depacketizer_impl()
      : gr::block("dstar_depacketizer",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(0, 0, 0))
    {
		bitbuffer = 0;
		state = RX_DSTAR_OUT_OF_SYNC;
		data_frame_counter = 0;
		message_port_register_out(pmt::mp("out"));
		
    }

    /*
     * Our virtual destructor.
     */
    dstar_depacketizer_impl::~dstar_depacketizer_impl()
    {
    }

    void
    dstar_depacketizer_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
		switch(state) {

			case RX_DSTAR_OUT_OF_SYNC: {
				ninput_items_required[0] = 1;
			}
			break;
			case RX_DSTAR_SEARCH_FOR_SYNCFLAG: {
				ninput_items_required[0] = 1;
			}
			break;
			case RX_DSTAR_HEADER: {
				ninput_items_required[0] = 660;
			}
			break;
			case RX_DSTAR_VOICE: {	
				ninput_items_required[0] = 72;
			}
			break;
			case RX_DSTAR_VOICE_AFTER_HEADER: {
				ninput_items_required[0] = 72;
			}
			break;
			case RX_DSTAR_DATA: {
				ninput_items_required[0] = 48; // normally data is 24, but with 28 the termination flag can be checked.
			}
			break;
			case RX_DSTAR_TERMINATION: {
				ninput_items_required[0] = 48;
			}
			break;
			default:
				cout << "Error in statemachine" << endl;
		}
    }
    
	/* type 0 = PI
	* type 1 = PS
	* type 2 = PTY
	* type 3 = flagstring: TP, TA, MuSp, MoSt, AH, CMP, stPTY
	* type 4 = RadioText 
	* type 5 = ClockTime
	* type 6 = Alternative Frequencies */
	void dstar_depacketizer_impl::send_message(long msgtype, std::string msgtext){
		pmt::pmt_t msg = pmt::mp(msgtext);
		pmt::pmt_t type = pmt::from_long(msgtype);
		message_port_pub(pmt::mp("out"), pmt::make_tuple(type, msg));
		std::cout << "DSTAR: " << msgtext << std::endl;
	}
    
    
    
	unsigned short dstar_depacketizer_impl::rx_dstar_crc_header(const unsigned char* header)
	{
		// Generatorpolynom G(x) = x^16 + x^12 + x^5 + 1
		// ohne die fuehrende 1 UND in umgekehrter Reihenfolge
		register const unsigned short genpoly = 0x8408;

		register unsigned short crc = 0xffff;

		for (char i=0; i<39; ++i)
		{
			crc ^= *header++;
			for (char j=0; j<8; ++j)
			{
			  if ( crc & 0x1 ) 
			  {
				crc >>= 1;
				crc ^= genpoly;
			  } 
			  else 
			  {
				crc >>= 1;
			  }
			}
		}

		// Beachte die Reihenfolge der CRC-Bytes!!!
		// Zunaechst kommt Low- und dann High-Byte
		// in "LSB first" Reihenfolge.
		return (crc ^ 0xffff);        // invertiere das Ergebnis
	}
    
    
	void
	dstar_depacketizer_impl::processSlowData(uint16_t number, const uint8_t* data)
	{
		
	  uint8_t header;
		
	  const uint8_t scramble[] =
	  {
		SCRAMBLER_BYTE1,
		SCRAMBLER_BYTE2,
		SCRAMBLER_BYTE3
	  };

	  if (number & RP2C_NUMBER_MASK)
	  {
		if (number & 1)
		  header = *data ^ SCRAMBLER_BYTE1;

		switch (header & SLOW_DATA_TYPE_MASK)
		{
		  case SLOW_DATA_TYPE_TEXT:
			{
			  size_t part = header & SLOW_DATA_TEXT_PART_MASK;
			  size_t position = part * SLOW_DATA_DATA_SIZE;
			  position += (~number & 1) * (SLOW_DATA_FRAME_SIZE - 1);
			  cout << endl;
			  for (size_t index = number & 1; index < SLOW_DATA_FRAME_SIZE; index ++)
			  {
				char letter = data[index] ^ scramble[index];
				cout << letter;
				//text[position] = (letter > ' ') ? letter : ' ';
				position ++;
			  }
			  cout << endl;
			}
			break;
/*
		  case SLOW_DATA_TYPE_GPS:
			if (type == LOCATION_TYPE_UNKNOWN)
			{
			  ssize_t size = header & SLOW_DATA_LENGTH_MASK;
			  size -= (~number & 1) * SLOW_DATA_FRAME_SIZE;
			  for (ssize_t index = number & 1; (index < SLOW_DATA_FRAME_SIZE) &&
				  (index <= size) && (position < POSITION_BUFFER_LENGTH); index ++)
			  {
				char letter = data[index] ^ scramble[index];
				if ((letter >= ' ') || (letter == '\r'))
				{
				  location[position] = letter;
				  position ++;
				}
				if (letter == '\r')
				{
				  validateLocation();
				  position = 0;
				  break;
				}
			  }
			}
			break;*/
		}
	  }
	}
	
	void
	dstar_depacketizer_impl::validateLocation()
	{/*
	  if (position > SLOW_DATA_DPRS_SIGN_SIZE)
	  {
		char* packet = location + SLOW_DATA_DPRS_SIGN_SIZE;
		size_t length = position - SLOW_DATA_DPRS_SIGN_SIZE;
		uint16_t sum = CalculateCCITTCheckSum(packet, length);
		char buffer[SLOW_DATA_DPRS_SIGN_SIZE + 1];
		sprintf(buffer, "$$CRC%04X,", sum);
		if (memcmp(location, buffer, SLOW_DATA_DPRS_SIGN_SIZE) == 0)
		{
		  position --;
		  location[position] = '\0';
		  type = LOCATION_TYPE_APRS;
		}
	  }*/
	}
    

    int
    dstar_depacketizer_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
    const unsigned char *in = (const unsigned char *) input_items[0];

	int samples_consumed = 0;

		switch(state) {

		case RX_DSTAR_OUT_OF_SYNC: {
			// shiftregister for the binary datastream
			bitbuffer <<= 1;
			if (*in == 0x01){
				bitbuffer |= 0x01;
			}
			// frame sync detector
			if ((bitbuffer & 0xFFFFFFFF) == FRAME_SYNC){
				cout << "<frame_sync>" << endl;
				state = RX_DSTAR_HEADER;
				data_frame_counter = 0;
			}
			
			// sync flag detector
			if ((bitbuffer & 0x00FFFFFF) == SYNC_FLAG){
				cout << "<sync_flag>" << endl;
				state = RX_DSTAR_VOICE;
				data_frame_counter = 0;
			}
			
			samples_consumed = 1;
		}
		break;
		case RX_DSTAR_SEARCH_FOR_SYNCFLAG: {
			// shiftregister for the binary datastream
			bitbuffer <<= 1;
			if (*in == 0x01){
				bitbuffer |= 0x01;
			}

			// sync flag detector
			if ((bitbuffer & 0x00FFFFFF) == SYNC_FLAG){
				cout << "<sync_flag>" << endl;
				state = RX_DSTAR_VOICE;
				//state = RX_DSTAR_HEADER;
				data_frame_counter = 0;
			}

			samples_consumed = 1;
		}
		break;
		case RX_DSTAR_HEADER: {
			
			for ( int x = 0; x < 660; x++)
			{
				raw_header[x] = in[x];
			}
			
			headerDecode(raw_header, true, &header);
			
			cout << "D-STAR Header: " << endl;
       
			cout << "Flag 1, upper Bits: ";
			if ( header.isData )
				cout << "data | ";
			else
				cout << "voice | ";

			if ( header.viaRepeater )
				cout << "via repeater | ";
			else
				cout << "direct | ";

			if ( header.interruption )
				cout << "interruption exists | ";
			else
				cout << "no interruption | ";

			if ( header.isControlSignal )
				cout << "control signal | ";
			else
				cout << "regular data | ";

			if ( header.emergency )
				cout << "EMERGENCY";
			else
				cout << "no emergency";
				
			cout << endl;
			
			
			cout << "Flag 1, lower Bits: ";
			if ((header.flag1 & 0x07) == 0x00)
			  cout << "NULL";
			
			if ((header.flag1 & 0x07) == 0x01)
			  cout << "Relay unavailable";

			if ((header.flag1 & 0x07) == 0x02)
			  cout << "No Response";
			
			if ((header.flag1 & 0x07) == 0x03)
			  cout << "ACK";
			
			if ((header.flag1 & 0x07) == 0x04)
			  cout << "Resend";

			if ((header.flag1 & 0x07) == 0x05)
			  cout << "Unused (spare)";
			
			if ((header.flag1 & 0x07) == 0x06)
			  cout << "Auto Reply" << endl;

			if ((header.flag1 & 0x07) == 0x07)
			  cout << "Repeater Control";

			cout << endl;
			
			cout << "Flag 2: ";
			for (int i = 7; i >= 0; --i)
			{
				if ( header.flag2 & (1 << i) )
				{
					cout << "1";
				}
				else
				{
					cout << "0";
				}
			}
			cout << " : for future expandability (MSB ... LSB)" << endl;
			
			
			cout << "Flag 3: ";
			for (int i = 7; i >= 0; --i)
			{
				if ( header.flag3 & (1 << i) )
				{
					cout << "1";
				}
				else
				{
					cout << "0";
				}
			}
			cout << " : for future expandability (MSB ... LSB)" << endl;
			
			cout << "Destination Repeater: " << header.destinationRepeater << endl;
			cout << "Departure Repeater: " << header.departureRepeater << endl;
			cout << "Companion Callsign: " << header.companionCallsign << endl;
			cout << "Callsign: " << header.ownCallsign << endl;
			cout << "Suffix: " << header.ownCallsignSuffix << endl;
			cout << "Received Checksum: " << hex << (short)header.checksum << endl;
			cout << "Calculated Checksum: " << hex << (short)header.calculatedChecksum << endl;
			
			if ( header.checksum != header.calculatedChecksum )
			{
				cout << "Checksum WRONG" << endl;
			}
			else 
			{
				cout << "Checksum OK" << endl;
			}
			
			
			//cout << "<header>";
			state = RX_DSTAR_VOICE_AFTER_HEADER;
			//state = RX_DSTAR_OUT_OF_SYNC;
			samples_consumed = 660;
			
			/* sending the messages to the queue */
			send_message(1,"HEADERTEST");
			//send_message(3,flagstring);
			//send_message(6,af_string);
		}
		break;
		case RX_DSTAR_VOICE: {
			
			memcpy(&raw_voice[0], &in[0], 72);
			//for ( int x = 0; x < 72; x++)
			//{
			//	raw_voice[x] = in[x];
			//}
			cout << "<voice>";
			state = RX_DSTAR_DATA;
			samples_consumed = 72;
		}
		break;
		case RX_DSTAR_VOICE_AFTER_HEADER: {
			
			memcpy(&raw_voice[0], &in[0], 72);
			//for ( int x = 0; x < 72; x++)
			//{
			//	raw_voice[x] = in[x];
			//}
			cout << "<voice>";
			state = RX_DSTAR_SEARCH_FOR_SYNCFLAG;
			samples_consumed = 72;
		}
		break;
		case RX_DSTAR_DATA: {
			
			unsigned int shiftregister = 0;
			for ( int i = 0; i < 48; i++)
			{
				// shiftregister for the binary datastream
				shiftregister <<= 1;
				if (in[i] == 0x01){
					shiftregister |= 0x01;
				}
				
			}
			// termination flag detector
			if ((shiftregister & 0xFFFFFFFF) == TERMINATION_FLAG){
				state = RX_DSTAR_TERMINATION;
			}
			else
			{
				for (int x = 0; x < 24; x++)
				{
					raw_data[x] = in[x];
					raw_slowspeed_data[x+(24*data_frame_counter)] = raw_data[x];
				}
				cout << "<data>";
				data_frame_counter++;
				if ( data_frame_counter < 21 )
				{
					state = RX_DSTAR_VOICE;
				}
				else 
				{
					data_frame_counter = 0;
					state = RX_DSTAR_SEARCH_FOR_SYNCFLAG;
				}
				samples_consumed = 24;
			}
		}
		break;
		case RX_DSTAR_TERMINATION: {
			state = RX_DSTAR_OUT_OF_SYNC;
			cout << "<termination>" << endl;
			samples_consumed = 48;
		}
		break;
		default:
			cout << "Error in state machine" << endl;
	}

	

	


	// tell the scheduler that we have consumed 1 sample
	consume_each(samples_consumed);
	

        // Tell runtime system how many output items we produced.
	//        return noutput_items;
	return 0;
    }

  } /* namespace OE2AIP */
} /* namespace gr */

