/* -*- c++ -*- */

#define OE2AIP_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "OE2AIP_swig_doc.i"

%{
#include "OE2AIP/simple_fm_demod_cf.h"
#include "OE2AIP/dstar_depacketizer.h"
#include "OE2AIP/uart_decoder.h"
#include "OE2AIP/uart_decoder_from_sync.h"
#include "OE2AIP/dstar_header_decoder.h"
#include "OE2AIP/dstar_dd_mode.h"
#include "OE2AIP/uart_decoder.h"
#include "OE2AIP/uart_decoder_from_sync.h"
#include "OE2AIP/dstar_header_decoder.h"
#include "OE2AIP/dstar_dd_mode.h"
%}

%include "OE2AIP/simple_fm_demod_cf.h"
GR_SWIG_BLOCK_MAGIC2(OE2AIP, simple_fm_demod_cf);
%include "OE2AIP/dstar_depacketizer.h"
GR_SWIG_BLOCK_MAGIC2(OE2AIP, dstar_depacketizer);
%include "OE2AIP/uart_decoder.h"
GR_SWIG_BLOCK_MAGIC2(OE2AIP, uart_decoder);
%include "OE2AIP/uart_decoder_from_sync.h"
GR_SWIG_BLOCK_MAGIC2(OE2AIP, uart_decoder_from_sync);
%include "OE2AIP/dstar_header_decoder.h"
GR_SWIG_BLOCK_MAGIC2(OE2AIP, dstar_header_decoder);
%include "OE2AIP/dstar_dd_mode.h"
GR_SWIG_BLOCK_MAGIC2(OE2AIP, dstar_dd_mode);
