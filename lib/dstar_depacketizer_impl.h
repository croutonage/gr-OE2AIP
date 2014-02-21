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

namespace gr {
  namespace OE2AIP {

    class dstar_depacketizer_impl : public dstar_depacketizer
    {
     private:
      bool frame_sync_detected;
      char raw_header[660];
      int header_counter;

      bool first_voice_started;
      char first_voice[72];
      int first_voice_counter;

      bool sync_detected;
      char raw_voice_and_data[72 + 24];
      int voice_and_data_counter;

     public:
      dstar_depacketizer_impl();
      ~dstar_depacketizer_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace OE2AIP
} // namespace gr

#endif /* INCLUDED_OE2AIP_DSTAR_DEPACKETIZER_IMPL_H */

