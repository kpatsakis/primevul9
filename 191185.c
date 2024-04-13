qtdemux_parse_tfdt (GstQTDemux * qtdemux, GstByteReader * br,
    guint64 * decode_time)
{
  guint32 version = 0;

  if (!gst_byte_reader_get_uint32_be (br, &version))
    return FALSE;

  version >>= 24;
  if (version == 1) {
    if (!gst_byte_reader_get_uint64_be (br, decode_time))
      goto failed;
  } else {
    guint32 dec_time = 0;
    if (!gst_byte_reader_get_uint32_be (br, &dec_time))
      goto failed;
    *decode_time = dec_time;
  }

  GST_INFO_OBJECT (qtdemux, "Track fragment decode time: %" G_GUINT64_FORMAT,
      *decode_time);

  return TRUE;

failed:
  {
    GST_DEBUG_OBJECT (qtdemux, "parsing tfdt failed");
    return FALSE;
  }
}