gst_aac_parse_adts_get_frame_len (const guint8 * data)
{
  return ((data[3] & 0x03) << 11) | (data[4] << 3) | ((data[5] & 0xe0) >> 5);
}