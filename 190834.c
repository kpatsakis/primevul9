gst_aac_parse_prepend_adts_headers (GstAacParse * aacparse,
    GstBaseParseFrame * frame)
{
  GstMemory *mem;
  guint8 *adts_headers;
  gsize buf_size;
  gsize frame_size;
  guint8 id, profile, channel_configuration, sampling_frequency_index;

  id = (aacparse->mpegversion == 4) ? 0x0U : 0x1U;
  profile = gst_aac_parse_get_audio_profile_object_type (aacparse);
  if (profile == G_MAXUINT8) {
    GST_ERROR_OBJECT (aacparse, "Unsupported audio profile or object type");
    return FALSE;
  }
  channel_configuration =
      gst_aac_parse_get_audio_channel_configuration (aacparse->channels);
  if (channel_configuration == G_MAXUINT8) {
    GST_ERROR_OBJECT (aacparse, "Unsupported number of channels");
    return FALSE;
  }
  sampling_frequency_index =
      gst_aac_parse_get_audio_sampling_frequency_index (aacparse->sample_rate);
  if (sampling_frequency_index == G_MAXUINT8) {
    GST_ERROR_OBJECT (aacparse, "Unsupported sampling frequency");
    return FALSE;
  }

  frame->out_buffer = gst_buffer_copy (frame->buffer);
  buf_size = gst_buffer_get_size (frame->out_buffer);
  frame_size = buf_size + ADTS_HEADERS_LENGTH;

  if (G_UNLIKELY (frame_size >= 0x4000)) {
    GST_ERROR_OBJECT (aacparse, "Frame size is too big for ADTS");
    return FALSE;
  }

  adts_headers = (guint8 *) g_malloc0 (ADTS_HEADERS_LENGTH);

  /* Note: no error correction bits are added to the resulting ADTS frames */
  adts_headers[0] = 0xFFU;
  adts_headers[1] = 0xF0U | (id << 3) | 0x1U;
  adts_headers[2] = (profile << 6) | (sampling_frequency_index << 2) | 0x2U |
      (channel_configuration & 0x4U);
  adts_headers[3] = ((channel_configuration & 0x3U) << 6) | 0x30U |
      (guint8) (frame_size >> 11);
  adts_headers[4] = (guint8) ((frame_size >> 3) & 0x00FF);
  adts_headers[5] = (guint8) (((frame_size & 0x0007) << 5) + 0x1FU);
  adts_headers[6] = 0xFCU;

  mem = gst_memory_new_wrapped (0, adts_headers, ADTS_HEADERS_LENGTH, 0,
      ADTS_HEADERS_LENGTH, adts_headers, g_free);
  gst_buffer_prepend_memory (frame->out_buffer, mem);

  return TRUE;
}