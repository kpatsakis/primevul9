gst_aac_parse_loas_get_frame_len (const guint8 * data)
{
  return (((data[1] & 0x1f) << 8) | data[2]) + 3;
}