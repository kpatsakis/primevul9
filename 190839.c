gst_aac_parse_get_audio_sample_rate (GstAacParse * aacparse, GstBitReader * br,
    gint * sample_rate)
{
  guint8 sampling_frequency_index;
  if (!gst_bit_reader_get_bits_uint8 (br, &sampling_frequency_index, 4))
    return FALSE;
  GST_LOG_OBJECT (aacparse, "sampling_frequency_index: %u",
      sampling_frequency_index);
  if (sampling_frequency_index == 0xf) {
    guint32 sampling_rate;
    if (!gst_bit_reader_get_bits_uint32 (br, &sampling_rate, 24))
      return FALSE;
    *sample_rate = sampling_rate;
  } else {
    *sample_rate = loas_sample_rate_table[sampling_frequency_index];
    if (!*sample_rate)
      return FALSE;
  }
  aacparse->last_parsed_sample_rate = *sample_rate;
  return TRUE;
}