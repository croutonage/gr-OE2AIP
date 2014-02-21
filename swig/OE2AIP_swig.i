/* -*- c++ -*- */

#define OE2AIP_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "OE2AIP_swig_doc.i"

%{
#include "OE2AIP/simple_fm_demod_cf.h"
#include "OE2AIP/dstar_depacketizer.h"
%}

%include "OE2AIP/simple_fm_demod_cf.h"
GR_SWIG_BLOCK_MAGIC2(OE2AIP, simple_fm_demod_cf);
%include "OE2AIP/dstar_depacketizer.h"
GR_SWIG_BLOCK_MAGIC2(OE2AIP, dstar_depacketizer);
