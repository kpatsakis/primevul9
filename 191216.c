qtdemux_parse_mehd (GstQTDemux * qtdemux, GstByteReader * br)
{
  guint32 version = 0;
  GstClockTime duration = 0;

  if (!gst_byte_reader_get_uint32_be (br, &version))
    goto failed;

  version >>= 24;
  if (version == 1) {
    if (!gst_byte_reader_get_uint64_be (br, &duration))
      goto failed;
  } else {
    guint32 dur = 0;

    if (!gst_byte_reader_get_uint32_be (br, &dur))
      goto failed;
    duration = dur;
  }

  GST_INFO_OBJECT (qtdemux, "mehd duration: %" G_GUINT64_FORMAT, duration);
  qtdemux->duration = duration;

  return TRUE;

failed:
  {
    GST_DEBUG_OBJECT (qtdemux, "parsing mehd failed");
    return FALSE;
  }
}