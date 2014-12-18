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

#ifndef INCLUDED_OE2AIP_DSTAR_DEPACKETIZER_IMPL_H
#define INCLUDED_OE2AIP_DSTAR_DEPACKETIZER_IMPL_H

#include <OE2AIP/dstar_depacketizer.h>
#include "dstar_header_decoder.h"

namespace gr {
  namespace OE2AIP {

    class dstar_depacketizer_impl : public dstar_depacketizer
    {
     private:
      unsigned int bitbuffer;


	typedef enum {
		RX_DSTAR_OUT_OF_SYNC,
		RX_DSTAR_SEARCH_FOR_SYNCFLAG,
		RX_DSTAR_HEADER,
		RX_DSTAR_VOICE,
		RX_DSTAR_VOICE_AFTER_HEADER,
		RX_DSTAR_DATA,
		RX_DSTAR_TERMINATION
	} rx_dstar_state_t;

      rx_dstar_state_t state;
      
      header_t header;

      unsigned char raw_header[660];
      unsigned char raw_slowspeed_data[480];
      unsigned char raw_voice[72];
      unsigned char raw_data[24];
      
      unsigned char dstar_sd_header[41];
      
      
      int header_counter;
      int voice_counter;
      int data_counter;
      int data_frame_counter;

      bool sync_detected;
      char raw_voice_and_data[72 + 24];
      int voice_and_data_counter;

     public:
      dstar_depacketizer_impl();
      ~dstar_depacketizer_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);
	  void send_message(long msgtype, std::string msgtext);
	  void processSlowData(uint16_t number, const uint8_t* data);
	  void validateLocation();

	  unsigned short rx_dstar_crc_header(const unsigned char* header);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace OE2AIP
} // namespace gr

#endif /* INCLUDED_OE2AIP_DSTAR_DEPACKETIZER_IMPL_H */

