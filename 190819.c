gst_aac_parse_set_src_caps (GstAacParse * aacparse, GstCaps * sink_caps)
{
  GstStructure *s;
  GstCaps *src_caps = NULL, *allowed;
  gboolean res = FALSE;
  const gchar *stream_format;
  guint8 codec_data[2];
  guint16 codec_data_data;
  gint sample_rate_idx;

  GST_DEBUG_OBJECT (aacparse, "sink caps: %" GST_PTR_FORMAT, sink_caps);
  if (sink_caps)
    src_caps = gst_caps_copy (sink_caps);
  else
    src_caps = gst_caps_new_empty_simple ("audio/mpeg");

  gst_caps_set_simple (src_caps, "framed", G_TYPE_BOOLEAN, TRUE,
      "mpegversion", G_TYPE_INT, aacparse->mpegversion, NULL);

  aacparse->output_header_type = aacparse->header_type;
  switch (aacparse->header_type) {
    case DSPAAC_HEADER_NONE:
      stream_format = "raw";
      break;
    case DSPAAC_HEADER_ADTS:
      stream_format = "adts";
      break;
    case DSPAAC_HEADER_ADIF:
      stream_format = "adif";
      break;
    case DSPAAC_HEADER_LOAS:
      stream_format = "loas";
      break;
    default:
      stream_format = NULL;
  }

  /* Generate codec data to be able to set profile/level on the caps */
  sample_rate_idx =
      gst_codec_utils_aac_get_index_from_sample_rate (aacparse->sample_rate);
  if (sample_rate_idx < 0)
    goto not_a_known_rate;
  codec_data_data =
      (aacparse->object_type << 11) |
      (sample_rate_idx << 7) | (aacparse->channels << 3);
  GST_WRITE_UINT16_BE (codec_data, codec_data_data);
  gst_codec_utils_aac_caps_set_level_and_profile (src_caps, codec_data, 2);

  s = gst_caps_get_structure (src_caps, 0);
  if (aacparse->sample_rate > 0)
    gst_structure_set (s, "rate", G_TYPE_INT, aacparse->sample_rate, NULL);
  if (aacparse->channels > 0)
    gst_structure_set (s, "channels", G_TYPE_INT, aacparse->channels, NULL);
  if (stream_format)
    gst_structure_set (s, "stream-format", G_TYPE_STRING, stream_format, NULL);

  allowed = gst_pad_get_allowed_caps (GST_BASE_PARSE (aacparse)->srcpad);
  if (allowed && !gst_caps_can_intersect (src_caps, allowed)) {
    GST_DEBUG_OBJECT (GST_BASE_PARSE (aacparse)->srcpad,
        "Caps can not intersect");
    if (aacparse->header_type == DSPAAC_HEADER_ADTS) {
      GST_DEBUG_OBJECT (GST_BASE_PARSE (aacparse)->srcpad,
          "Input is ADTS, trying raw");
      gst_caps_set_simple (src_caps, "stream-format", G_TYPE_STRING, "raw",
          NULL);
      if (gst_caps_can_intersect (src_caps, allowed)) {
        GstBuffer *codec_data_buffer;

        GST_DEBUG_OBJECT (GST_BASE_PARSE (aacparse)->srcpad,
            "Caps can intersect, we will drop the ADTS layer");
        aacparse->output_header_type = DSPAAC_HEADER_NONE;

        /* The codec_data data is according to AudioSpecificConfig,
           ISO/IEC 14496-3, 1.6.2.1 */
        codec_data_buffer = gst_buffer_new_and_alloc (2);
        gst_buffer_fill (codec_data_buffer, 0, codec_data, 2);
        gst_caps_set_simple (src_caps, "codec_data", GST_TYPE_BUFFER,
            codec_data_buffer, NULL);
      }
    } else if (aacparse->header_type == DSPAAC_HEADER_NONE) {
      GST_DEBUG_OBJECT (GST_BASE_PARSE (aacparse)->srcpad,
          "Input is raw, trying ADTS");
      gst_caps_set_simple (src_caps, "stream-format", G_TYPE_STRING, "adts",
          NULL);
      if (gst_caps_can_intersect (src_caps, allowed)) {
        GST_DEBUG_OBJECT (GST_BASE_PARSE (aacparse)->srcpad,
            "Caps can intersect, we will prepend ADTS headers");
        aacparse->output_header_type = DSPAAC_HEADER_ADTS;
      }
    }
  }
  if (allowed)
    gst_caps_unref (allowed);

  aacparse->last_parsed_channels = 0;
  aacparse->last_parsed_sample_rate = 0;

  GST_DEBUG_OBJECT (aacparse, "setting src caps: %" GST_PTR_FORMAT, src_caps);

  res = gst_pad_set_caps (GST_BASE_PARSE (aacparse)->srcpad, src_caps);
  gst_caps_unref (src_caps);
  return res;

not_a_known_rate:
  GST_ERROR_OBJECT (aacparse, "Not a known sample rate: %d",
      aacparse->sample_rate);
  gst_caps_unref (src_caps);
  return FALSE;
}