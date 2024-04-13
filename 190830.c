gst_aac_parse_read_loas_config (GstAacParse * aacparse, const guint8 * data,
    guint avail, gint * sample_rate, gint * channels, gint * version)
{
  GstBitReader br;
  guint8 u8, v, vA;

  /* No version in the bitstream, but the spec has LOAS in the MPEG-4 section */
  if (version)
    *version = 4;

  gst_bit_reader_init (&br, data, avail);

  /* skip sync word (11 bits) and size (13 bits) */
  if (!gst_bit_reader_skip (&br, 11 + 13))
    return FALSE;

  /* First bit is "use last config" */
  if (!gst_bit_reader_get_bits_uint8 (&br, &u8, 1))
    return FALSE;
  if (u8) {
    GST_LOG_OBJECT (aacparse, "Frame uses previous config");
    if (!aacparse->last_parsed_sample_rate || !aacparse->last_parsed_channels) {
      GST_DEBUG_OBJECT (aacparse,
          "No previous config to use. We'll look for more data.");
      return FALSE;
    }
    *sample_rate = aacparse->last_parsed_sample_rate;
    *channels = aacparse->last_parsed_channels;
    return TRUE;
  }

  GST_DEBUG_OBJECT (aacparse, "Frame contains new config");

  /* audioMuxVersion */
  if (!gst_bit_reader_get_bits_uint8 (&br, &v, 1))
    return FALSE;
  if (v) {
    /* audioMuxVersionA */
    if (!gst_bit_reader_get_bits_uint8 (&br, &vA, 1))
      return FALSE;
  } else
    vA = 0;

  GST_LOG_OBJECT (aacparse, "v %d, vA %d", v, vA);
  if (vA == 0) {
    guint8 same_time, subframes, num_program, prog;
    if (v == 1) {
      guint32 value;
      /* taraBufferFullness */
      if (!gst_aac_parse_latm_get_value (aacparse, &br, &value))
        return FALSE;
    }
    if (!gst_bit_reader_get_bits_uint8 (&br, &same_time, 1))
      return FALSE;
    if (!gst_bit_reader_get_bits_uint8 (&br, &subframes, 6))
      return FALSE;
    if (!gst_bit_reader_get_bits_uint8 (&br, &num_program, 4))
      return FALSE;
    GST_LOG_OBJECT (aacparse, "same_time %d, subframes %d, num_program %d",
        same_time, subframes, num_program);

    for (prog = 0; prog <= num_program; ++prog) {
      guint8 num_layer, layer;
      if (!gst_bit_reader_get_bits_uint8 (&br, &num_layer, 3))
        return FALSE;
      GST_LOG_OBJECT (aacparse, "Program %d: %d layers", prog, num_layer);

      for (layer = 0; layer <= num_layer; ++layer) {
        guint8 use_same_config;
        if (prog == 0 && layer == 0) {
          use_same_config = 0;
        } else {
          if (!gst_bit_reader_get_bits_uint8 (&br, &use_same_config, 1))
            return FALSE;
        }
        if (!use_same_config) {
          if (v == 0) {
            if (!gst_aac_parse_read_loas_audio_specific_config (aacparse, &br,
                    sample_rate, channels, NULL))
              return FALSE;
          } else {
            guint32 bits, asc_len;
            if (!gst_aac_parse_latm_get_value (aacparse, &br, &asc_len))
              return FALSE;
            if (!gst_aac_parse_read_loas_audio_specific_config (aacparse, &br,
                    sample_rate, channels, &bits))
              return FALSE;
            asc_len -= bits;
            if (!gst_bit_reader_skip (&br, asc_len))
              return FALSE;
          }
        }
      }
    }
    GST_LOG_OBJECT (aacparse, "More data ignored");
  } else {
    GST_WARNING_OBJECT (aacparse, "Spec says \"TBD\"...");
    return FALSE;
  }
  return TRUE;
}