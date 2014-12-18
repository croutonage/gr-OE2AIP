#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "dstar_dd_mode_impl.h"
#include "dstar_header_decoder.h"

#define FRAME_SYNC 0x55557650
#define SYNC_FLAG 0x00AAB468
#define TERMINATION_FLAG 0xAAAA135E

namespace gr {
  namespace OE2AIP {
  	using namespace std;

    dstar_dd_mode::sptr
    dstar_dd_mode::make()
    {
      return gnuradio::get_initial_sptr
        (new dstar_dd_mode_impl());
    }

    /*
     * The private constructor
     */
    dstar_dd_mode_impl::dstar_dd_mode_impl()
      : gr::block("dstar_dd_mode",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(0, 0, 0))
    {
		bitbuffer = 0;
		state = RX_DSTAR_OUT_OF_SYNC;

    }

    /*
     * Our virtual destructor.
     */
    dstar_dd_mode_impl::~dstar_dd_mode_impl()
    {
    }

    void
    dstar_dd_mode_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        switch(state) {

			case RX_DSTAR_OUT_OF_SYNC: {
				ninput_items_required[0] = 1;
			}
			break;
			case RX_DSTAR_HEADER: {
				ninput_items_required[0] = 660;
			}
			break;
			case RX_DSTAR_DD_DATA: {	
				ninput_items_required[0] = 16;
			}
			break;
			default:
				cout << "Error in statemachine" << endl;
		}
    }

    int
    dstar_dd_mode_impl::general_work (int noutput_items,
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
				}
				
				samples_consumed = 1;
			}
			break;
			case RX_DSTAR_HEADER: {

				//cout << "start: " << endl;
				for ( int x = 0; x < 660; x++)
				{
					raw_header[x] = in[x];
					//cout << hex << (int)raw_header[x] << ", ";
				}
				//_dstar_dd_header.decode(raw_header);
				cout << "<header>" << endl;
				state = RX_DSTAR_OUT_OF_SYNC;
				samples_consumed = 660;
			}
			break;
			case RX_DSTAR_DD_DATA: {
				/*
				for ( int x = 0; x < 72; x++)
				{
					raw_voice[x] = in[x];
				}
				cout << "<voice>";
				state = RX_DSTAR_DATA;
				samples_consumed = 72;*/
			}
			break;
			default:
				cout << "Error in state machine" << endl;
		}

	

	


	// tell the scheduler that we have consumed 1 sample
	consume_each(samples_consumed);

        // Tell runtime system how many output items we produced.
        return 0;
    }

  } /* namespace OE2AIP */
} /* namespace gr */

