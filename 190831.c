gst_aac_parse_parse_adts_header (GstAacParse * aacparse, const guint8 * data,
    gint * rate, gint * channels, gint * object, gint * version)
{

  if (rate) {
    gint sr_idx = (data[2] & 0x3c) >> 2;

    *rate = gst_codec_utils_aac_get_sample_rate_from_index (sr_idx);
  }
  if (channels) {
    *channels = ((data[2] & 0x01) << 2) | ((data[3] & 0xc0) >> 6);
    if (*channels == 7)
      *channels = 8;
  }

  if (version)
    *version = (data[1] & 0x08) ? 2 : 4;
  if (object)
    *object = ((data[2] & 0xc0) >> 6) + 1;
}