int LibRaw::get_decoder_info(libraw_decoder_info_t* d_info)
{
  if(!d_info)   return LIBRAW_UNSPECIFIED_ERROR;
  if(!load_raw) return LIBRAW_OUT_OF_ORDER_CALL;
  
  d_info->decoder_flags = LIBRAW_DECODER_NOTSET;
  int rawdata = (imgdata.idata.filters || P1.colors == 1);
  // dcraw.c names order
  if (load_raw == &LibRaw::canon_600_load_raw) 
    {
      d_info->decoder_name = "canon_600_load_raw()";   
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD; // WB set within decoder, no need to load raw
    }
  else if (load_raw == &LibRaw::canon_load_raw)
    {
      d_info->decoder_name = "canon_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else if (load_raw == &LibRaw::lossless_jpeg_load_raw)
    {
      // Check rbayer
      d_info->decoder_name = "lossless_jpeg_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD | LIBRAW_DECODER_HASCURVE | LIBRAW_DECODER_TRYRAWSPEED;
    }
  else if (load_raw == &LibRaw::canon_sraw_load_raw) 
    {
      d_info->decoder_name = "canon_sraw_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_LEGACY | LIBRAW_DECODER_TRYRAWSPEED; 
    }
  else if (load_raw == &LibRaw::lossless_dng_load_raw) 
    {
      // Check rbayer
      d_info->decoder_name = "lossless_dng_load_raw()"; 
      d_info->decoder_flags = rawdata? LIBRAW_DECODER_FLATFIELD : LIBRAW_DECODER_LEGACY ;
      d_info->decoder_flags |= LIBRAW_DECODER_HASCURVE;
      d_info->decoder_flags |= LIBRAW_DECODER_TRYRAWSPEED;
    }
  else if (load_raw == &LibRaw::packed_dng_load_raw)
    {
      // Check rbayer
      d_info->decoder_name = "packed_dng_load_raw()"; 
      d_info->decoder_flags = rawdata ? LIBRAW_DECODER_FLATFIELD : LIBRAW_DECODER_LEGACY;
      d_info->decoder_flags |= LIBRAW_DECODER_HASCURVE;
      d_info->decoder_flags |= LIBRAW_DECODER_TRYRAWSPEED;
    }
  else if (load_raw == &LibRaw::pentax_load_raw )
    {
      d_info->decoder_name = "pentax_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD  | LIBRAW_DECODER_TRYRAWSPEED;
    }
  else if (load_raw == &LibRaw::nikon_load_raw)
    {
      // Check rbayer
      d_info->decoder_name = "nikon_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD | LIBRAW_DECODER_TRYRAWSPEED;
    }
  else if (load_raw == &LibRaw::rollei_load_raw )
    {
      // UNTESTED
      d_info->decoder_name = "rollei_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else if (load_raw == &LibRaw::phase_one_load_raw )
    {
      d_info->decoder_name = "phase_one_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else if (load_raw == &LibRaw::phase_one_load_raw_c )
    {
      d_info->decoder_name = "phase_one_load_raw_c()"; 
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else if (load_raw == &LibRaw::hasselblad_load_raw )
    {
      d_info->decoder_name = "hasselblad_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else if (load_raw == &LibRaw::leaf_hdr_load_raw )
    {
      d_info->decoder_name = "leaf_hdr_load_raw()"; 
      d_info->decoder_flags = imgdata.idata.filters? LIBRAW_DECODER_FLATFIELD:LIBRAW_DECODER_LEGACY;
    }
  else if (load_raw == &LibRaw::unpacked_load_raw )
    {
      d_info->decoder_name = "unpacked_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD | LIBRAW_DECODER_USEBAYER2;
    }
  else if (load_raw == &LibRaw::sinar_4shot_load_raw )
    {
      // UNTESTED
      d_info->decoder_name = "sinar_4shot_load_raw()";
      d_info->decoder_flags = (O.shot_select|| O.half_size)?LIBRAW_DECODER_FLATFIELD:LIBRAW_DECODER_LEGACY;
    }
  else if (load_raw == &LibRaw::imacon_full_load_raw )
    {
      d_info->decoder_name = "imacon_full_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_LEGACY; 
    }
  else if (load_raw == &LibRaw::hasselblad_full_load_raw )
    {
      d_info->decoder_name = "hasselblad_full_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_LEGACY; 
    }
  else if (load_raw == &LibRaw::packed_load_raw )
    {
      d_info->decoder_name = "packed_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
      d_info->decoder_flags |= LIBRAW_DECODER_TRYRAWSPEED;
    }
  else if (load_raw == &LibRaw::nokia_load_raw )
    {
      // UNTESTED
      d_info->decoder_name = "nokia_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else if (load_raw == &LibRaw::canon_rmf_load_raw )
    {
      // UNTESTED
      d_info->decoder_name = "canon_rmf_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else if (load_raw == &LibRaw::panasonic_load_raw )
    {
      d_info->decoder_name = "panasonic_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD  | LIBRAW_DECODER_TRYRAWSPEED;
    }
  else if (load_raw == &LibRaw::olympus_load_raw )
    {
      d_info->decoder_name = "olympus_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD | LIBRAW_DECODER_TRYRAWSPEED;
    }
  else if (load_raw == &LibRaw::minolta_rd175_load_raw ) 
    {  
      // UNTESTED
      d_info->decoder_name = "minolta_rd175_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else if (load_raw == &LibRaw::quicktake_100_load_raw )
    {
      // UNTESTED
      d_info->decoder_name = "quicktake_100_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else if (load_raw == &LibRaw::kodak_radc_load_raw )
    {
      d_info->decoder_name = "kodak_radc_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else if (load_raw == &LibRaw::kodak_jpeg_load_raw )
    {
      // UNTESTED + RBAYER
      d_info->decoder_name = "kodak_jpeg_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else if (load_raw == &LibRaw::lossy_dng_load_raw)
    {
      // Check rbayer
      d_info->decoder_name = "lossy_dng_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_LEGACY | LIBRAW_DECODER_TRYRAWSPEED;
      d_info->decoder_flags |= LIBRAW_DECODER_HASCURVE;
    }
  else if (load_raw == &LibRaw::kodak_dc120_load_raw )
    {
      d_info->decoder_name = "kodak_dc120_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else if (load_raw == &LibRaw::eight_bit_load_raw )
    {
      d_info->decoder_name = "eight_bit_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
      d_info->decoder_flags |= LIBRAW_DECODER_HASCURVE;
    }
  else if (load_raw == &LibRaw::kodak_yrgb_load_raw )    
    {
      d_info->decoder_name = "kodak_yrgb_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_LEGACY;
      d_info->decoder_flags |= LIBRAW_DECODER_HASCURVE;
    }
  else if (load_raw == &LibRaw::kodak_262_load_raw )
    {
      d_info->decoder_name = "kodak_262_load_raw()"; // UNTESTED!
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
      d_info->decoder_flags |= LIBRAW_DECODER_HASCURVE;
    }
  else if (load_raw == &LibRaw::kodak_65000_load_raw )
    {
      d_info->decoder_name = "kodak_65000_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
      d_info->decoder_flags |= LIBRAW_DECODER_HASCURVE;
    }
  else if (load_raw == &LibRaw::kodak_ycbcr_load_raw )
    {
      // UNTESTED
      d_info->decoder_name = "kodak_ycbcr_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_LEGACY;
      d_info->decoder_flags |= LIBRAW_DECODER_HASCURVE;
    }
  else if (load_raw == &LibRaw::kodak_rgb_load_raw ) 
    {
      // UNTESTED
      d_info->decoder_name = "kodak_rgb_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_LEGACY;
    }
  else if (load_raw == &LibRaw::sony_load_raw )
    {
      d_info->decoder_name = "sony_load_raw()"; 
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else if (load_raw == &LibRaw::sony_arw_load_raw )
    {
      d_info->decoder_name = "sony_arw_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
      d_info->decoder_flags |= LIBRAW_DECODER_TRYRAWSPEED;

    }
  else if (load_raw == &LibRaw::sony_arw2_load_raw )
    {
      d_info->decoder_name = "sony_arw2_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
      d_info->decoder_flags |= LIBRAW_DECODER_HASCURVE;
      d_info->decoder_flags |= LIBRAW_DECODER_TRYRAWSPEED;
      d_info->decoder_flags |= LIBRAW_DECODER_ITSASONY;
    }
  else if (load_raw == &LibRaw::samsung_load_raw )
    {
      d_info->decoder_name = "samsung_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
      d_info->decoder_flags |= LIBRAW_DECODER_TRYRAWSPEED;
    }
  else if (load_raw == &LibRaw::smal_v6_load_raw )
    {
      // UNTESTED
      d_info->decoder_name = "smal_v6_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else if (load_raw == &LibRaw::smal_v9_load_raw )
    {
      // UNTESTED
      d_info->decoder_name = "smal_v9_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD;
    }
  else  if (load_raw == &LibRaw::redcine_load_raw)
    {
      d_info->decoder_name = "redcine_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_FLATFIELD; 
      d_info->decoder_flags |= LIBRAW_DECODER_HASCURVE;
    }
  else if (load_raw == &LibRaw::x3f_load_raw )
    {
      d_info->decoder_name = "x3f_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_LEGACY | LIBRAW_DECODER_OWNALLOC; 
    }
#ifdef LIBRAW_DEMOSAIC_PACK_GPL2
  else if (load_raw == &LibRaw::foveon_sd_load_raw )
    {
      d_info->decoder_name = "foveon_sd_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_LEGACY; 
    }
  else if (load_raw == &LibRaw::foveon_dp_load_raw )
    {
      d_info->decoder_name = "foveon_dp_load_raw()";
      d_info->decoder_flags = LIBRAW_DECODER_LEGACY; 
    }
#endif
  else
    {
      d_info->decoder_name = "Unknown unpack function";
      d_info->decoder_flags = LIBRAW_DECODER_NOTSET;
    }
  return LIBRAW_SUCCESS;
}