qtdemux_parse_mfhd (GstQTDemux * qtdemux, GstByteReader * mfhd,
    guint32 * fragment_number)
{
  if (!gst_byte_reader_skip (mfhd, 4))
    goto fail;
  if (!gst_byte_reader_get_uint32_be (mfhd, fragment_number))
    goto fail;
  return TRUE;
fail:
  {
    GST_WARNING_OBJECT (qtdemux, "Failed to parse mfhd atom");
    return FALSE;
  }
}