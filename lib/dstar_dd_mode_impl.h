#ifndef INCLUDED_OE2AIP_DSTAR_DD_MODE_IMPL_H
#define INCLUDED_OE2AIP_DSTAR_DD_MODE_IMPL_H

#include <OE2AIP/dstar_dd_mode.h>
#include "dstar_header_decoder.h"

namespace gr {
  namespace OE2AIP {

    class dstar_dd_mode_impl : public dstar_dd_mode
    {
     private:

		unsigned char raw_header[660];
		unsigned int bitbuffer;

		typedef enum {
			RX_DSTAR_OUT_OF_SYNC,
			RX_DSTAR_HEADER,
			RX_DSTAR_DD_DATA
		} rx_dstar_state_t;
      // Nothing to declare in this block.

		 rx_dstar_state_t state;
		 header_t header;

     public:
      dstar_dd_mode_impl();
      ~dstar_dd_mode_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace OE2AIP
} // namespace gr

#endif /* INCLUDED_OE2AIP_DSTAR_DD_MODE_IMPL_H */

