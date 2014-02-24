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

	sync_detected = true;
	voice_and_data_counter = 0;
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
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
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



	for (size_t i = 0; i < noutput_items; i++) {
		 

		bitbuffer <<= 1;
		if (in[i] == 0x01){
			bitbuffer |= 0x01;
		}

/*
		// sync detector
		if ((bitbuffer & 0x00FFFFFF) == 0x00AAB468){
			cout << "sync detected";
		}
*/
		// frame sync detector
		if ((bitbuffer & 0xAAAAFFFF) == 0xAAAABB28){
//			cout << "frame sync detected: 0x" << hex << bitbuffer << " ";
			out0[i] = 0x02;
			header_counter = 0;	
			frame_sync_detected = true;	
		}
		else
		{
			if ( frame_sync_detected )
			{
				out0[i] = in[i];
				header_counter++;
			}
			else
			{
				header_counter = 0;
				out0[i] = 0x00;
			}
				
		}
		if ( header_counter > 660 )
			frame_sync_detected = false;

/*
		// termination flag detector
		if ((bitbuffer & 0xFFFFFFFF) == 0xAAAA135E){
			cout << "termination flag detected";
		}

*/
/*
		if ( (header_counter < 660) && frame_sync_detected ){
			raw_header[header_counter] = in0[i];
			header_counter++;
			consume(0, 1);
		 }


		if ( (first_voice_counter < 72 ) && first_voice_started){
		   	first_voice[first_voice_counter] = in0[i];
			first_voice_counter++;
			consume(0, 1);
		}

		if ( first_voice_counter == 72 )
		{
			first_voice_started = false;
			first_voice_counter = 0;
		
			cout << "First voice:" << endl;
                        	for (int x = 0; x < 72; x++ )
                                 	cout << hex << (int)first_voice[x] << ", ";
                         	cout << endl;

		}



		 // 0x02 from Correlate Access Code Module signals the start of the data
                 // The first byte with 0x02 contains no useable data
                 if ( (in0[i] & 0x02) == 0x02)
                 {
                         header_counter = 0;
                         frame_sync_detected = true;
			 cout << "Header started" << endl;
			 consume(0, 1);
                 }

                 if ( header_counter == 660)
                 {
                         frame_sync_detected = false;
                         header_counter = 0;
			 
                         //cout << "Outputitems: " << (int)noutput_items << endl;

                         cout << "Raw Header:" << endl;
                         for (int x = 0; x < 660; x++ )
                                 cout << hex << (int)raw_header[x] << ", ";
                         cout << endl;

			 // 72 bit after the header are voice bits
			 first_voice_started = true;
			 first_voice_counter = 0;
                 }

	 	 


  		 if ( (voice_and_data_counter < 72) && sync_detected )
		 {
                         raw_voice_and_data[voice_and_data_counter] = in1[i];
                         voice_and_data_counter++;
                         consume(1, 1);
                 }

		 // 0x02 from Correlate Access Code Module signals the start of the data
                 // The first byte with 0x02 contains no useable data
                 if ( (in1[i] & 0x02) == 0x02)
                 {
                         voice_and_data_counter = 0;
                         sync_detected = true;
                         cout << "Sync detected" << endl;
			 consume(1, 1);
                 }

                 if ( voice_and_data_counter == 72 )
                 {
                         sync_detected = false;
                         voice_and_data_counter = 0;

                         //cout << "Outputitems: " << (int)noutput_items << endl;

                         cout << "Raw voice:" << endl;
                         for (int x = 0; x < 72; x++ )
                                 cout << hex << (int)raw_voice_and_data[x] << ", ";
                         cout << endl;
                 }
		*/

	}
        // Do <+signal processing+>
        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each(noutput_items);
	//consume(0, 1);
	//consume(1, 1);

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace OE2AIP */
} /* namespace gr */

