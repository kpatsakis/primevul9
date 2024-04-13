bit_reader_skip_ebml_num (GstBitReader * br)
{
  guint8 i, v = 0;

  if (!gst_bit_reader_peek_bits_uint8 (br, &v, 8))
    return FALSE;

  for (i = 0; i < 8; i++) {
    if ((v & (0x80 >> i)) != 0)
      break;
  }
  return gst_bit_reader_skip (br, (i + 1) * 8);
}