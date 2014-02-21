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
#include "simple_fm_demod_cf_impl.h"

namespace gr {
  namespace OE2AIP {

    simple_fm_demod_cf::sptr
    simple_fm_demod_cf::make(float samp_rate)
    {
      return gnuradio::get_initial_sptr
        (new simple_fm_demod_cf_impl(samp_rate));
    }

    /*
     * The private constructor
     */
    simple_fm_demod_cf_impl::simple_fm_demod_cf_impl(float samp_rate)
      : gr::sync_block("simple_fm_demod_cf",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(float)))
    {
	samp_time = 1.0 / samp_rate;
	set_history(2);
    }

    /*
     * Our virtual destructor.
     */
    simple_fm_demod_cf_impl::~simple_fm_demod_cf_impl()
    {
    }

    int
    simple_fm_demod_cf_impl::work(int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
	in++;	// Set the pointer forward, so in[i-1] gets valid
        float *out = (float *) output_items[0];

    	float power;
	 
	for(int i = 0; i < noutput_items; i++)
        {
                power = imag(in[i]) * imag(in[i]) + real(in[i]) * real(in[i]);

                if (power > 1E-9)
                        out[i] = (real(in[i-1]) * imag(in[i]) - imag(in[i-1]) * real(in[i])) / ( 2.0 * M_PI * samp_time * power );
                else
                        out[i] = 0.0;
        }   

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace OE2AIP */
} /* namespace gr */

