/* -*- c++ -*- */
/* 
 * Copyright 2014 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "dstar_depacketizer_impl.h"

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
              gr::io_signature::make(3, 3, sizeof(char)))
    {
	bitbuffer = 0;
    	frame_sync_detected = false;
	header_counter = 0;
	
	first_voice_started = false;
	first_voice_counter = 0;

	sync_detected = false;
	voice_and_data_counter = 0;
	voice_frame_counter = 0;
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
	ninput_items_required[0] = noutput_items;
    }

    int
    dstar_depacketizer_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const char *in = (const char *) input_items[0];
        char *out0 = (char *) output_items[0];
	char *out1 = (char *) output_items[1];
 	char *out2 = (char *) output_items[2];


	int output_items_created = 0;

	for (size_t i = 0; i < noutput_items; i++) {
		 


		// shiftregister for the binary datastream
		bitbuffer <<= 1;
		if (in[i] == 0x01){
			bitbuffer |= 0x01;
		}

		// sync detector
		if ((bitbuffer & 0x00FFFFFF) == 0x00AAB468){
			cout << "<sync>";
			sync_detected = true;
			voice_and_data_counter = 0;
			voice_frame_counter = 0;
		}

		// frame sync detector
		if ((bitbuffer & 0xFFFFFFFF) == 0xAAAABB28){
			cout << "<frame_sync>";
			header_counter = 0;	
			frame_sync_detected = true;
			first_voice_counter = 0;	
		}
		else
		{
			// after the frame sync is received start the header output
			if ( frame_sync_detected )
			{
				// pass through the header data to the first output
				out0[i] = in[i];
				output_items_created++;
				header_counter++;
			}
		}

		// header is received
		if ( header_counter == 660 )
		{
			header_counter = 0;
			frame_sync_detected = false;
			cout << "<header>";
			
			// after the header is received, there follows one voice frame
			first_voice_started = true;
			first_voice_counter = 0;
		}


		// outputting the first voice to out1
		if ( first_voice_started && first_voice_counter < 72 )
		{
			// pass through the voice output to the second output
			out1[i] = in[i];
			output_items_created++;
			first_voice_counter++;
		}
		
		// first voice is ready
		if ( first_voice_counter == 72 )
		{
			cout << "<first voice>";
			first_voice_started = false;
			first_voice_counter = 0;
		}


		// shifting out the voice and data to out 1 and out 2
		if ( sync_detected && voice_and_data_counter < 96 )
		{
			// the first 72 bits are voice bits
			if ( voice_and_data_counter < 72 )
			{
				out1[i] = in[i];
			}
			if ( voice_and_data_counter == 72 )
			{
				voice_frame_counter++;
				cout << "<voice>";
			}
			// the 24 bits after the voice bits are data bits
			if ( voice_and_data_counter > 71 && voice_frame_counter < 21)
			{
				out2[i] = in[i];
			}
			if ( voice_and_data_counter == 95 )
				cout << "<data>";

			voice_and_data_counter++;
			output_items_created++;
		}

		// voice and data are 96 bit together
		if ( voice_and_data_counter == 96 )
		{
			voice_and_data_counter = 0;
		}

		// there are only 21 voice frames until the next sync word	
		if ( voice_frame_counter > 20 )
		{
			sync_detected = false;
			voice_frame_counter = 0;
		}

		// termination flag detector
		if ((bitbuffer & 0xFFFFFFFF) == 0xAAAA135E){
			cout << "<termination>" << endl;

        		frame_sync_detected = false;
        		header_counter = 0;

        		first_voice_started = false;
        		first_voice_counter = 0;

        		sync_detected = false;
        		voice_and_data_counter = 0;
        		voice_frame_counter = 0;
		}

		// tell the scheduler that we have consumed 1 sample
		consume_each(1);
	}

        // Tell runtime system how many output items we produced.
	//        return noutput_items;
	return output_items_created;
    }

  } /* namespace OE2AIP */
} /* namespace gr */

