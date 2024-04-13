gst_aac_parse_read_loas_audio_specific_config (GstAacParse * aacparse,
    GstBitReader * br, gint * sample_rate, gint * channels, guint32 * bits)
{
  guint8 audio_object_type;
  guint8 G_GNUC_UNUSED extension_audio_object_type;
  guint8 channel_configuration, extension_channel_configuration;
  gboolean G_GNUC_UNUSED sbr = FALSE, ps = FALSE;

  if (!gst_aac_parse_get_audio_object_type (aacparse, br, &audio_object_type))
    return FALSE;

  if (!gst_aac_parse_get_audio_sample_rate (aacparse, br, sample_rate))
    return FALSE;

  if (!gst_bit_reader_get_bits_uint8 (br, &channel_configuration, 4))
    return FALSE;
  *channels = loas_channels_table[channel_configuration];
  GST_LOG_OBJECT (aacparse, "channel_configuration: %d", channel_configuration);
  if (!*channels)
    return FALSE;

  if (audio_object_type == 5 || audio_object_type == 29) {
    extension_audio_object_type = 5;
    sbr = TRUE;
    if (audio_object_type == 29)
      ps = TRUE;

    GST_LOG_OBJECT (aacparse,
        "Audio object type 5 or 29, so rereading sampling rate...");
    if (!gst_aac_parse_get_audio_sample_rate (aacparse, br, sample_rate))
      return FALSE;

    if (!gst_aac_parse_get_audio_object_type (aacparse, br, &audio_object_type))
      return FALSE;

    if (audio_object_type == 22) {
      /* extension channel configuration */
      if (!gst_bit_reader_get_bits_uint8 (br, &extension_channel_configuration,
              4))
        return FALSE;
      GST_LOG_OBJECT (aacparse, "extension channel_configuration: %d",
          extension_channel_configuration);
      *channels = loas_channels_table[extension_channel_configuration];
      if (!*channels)
        return FALSE;
    }
  } else {
    extension_audio_object_type = 0;
  }

  GST_INFO_OBJECT (aacparse, "Found LOAS config: %d Hz, %d channels",
      *sample_rate, *channels);

  /* There's LOTS of stuff next, but we ignore it for now as we have
     what we want (sample rate and number of channels */
  GST_DEBUG_OBJECT (aacparse,
      "Need more code to parse humongous LOAS data, currently ignored");
  if (bits)
    *bits = 0;
  aacparse->last_parsed_channels = *channels;
  return TRUE;
}