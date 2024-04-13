gst_aac_parse_latm_get_value (GstAacParse * aacparse, GstBitReader * br,
    guint32 * value)
{
  guint8 bytes, i, byte;

  *value = 0;
  if (!gst_bit_reader_get_bits_uint8 (br, &bytes, 2))
    return FALSE;
  for (i = 0; i <= bytes; ++i) {
    *value <<= 8;
    if (!gst_bit_reader_get_bits_uint8 (br, &byte, 8))
      return FALSE;
    *value += byte;
  }
  return TRUE;
}