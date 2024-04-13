qtdemux_parse_transformation_matrix (GstQTDemux * qtdemux,
    GstByteReader * reader, guint32 * matrix, const gchar * atom)
{
  /*
   * 9 values of 32 bits (fixed point 16.16, except 2 5 and 8 that are 2.30)
   * [0 1 2]
   * [3 4 5]
   * [6 7 8]
   */

  if (gst_byte_reader_get_remaining (reader) < 36)
    return FALSE;

  matrix[0] = gst_byte_reader_get_uint32_be_unchecked (reader);
  matrix[1] = gst_byte_reader_get_uint32_be_unchecked (reader);
  matrix[2] = gst_byte_reader_get_uint32_be_unchecked (reader);
  matrix[3] = gst_byte_reader_get_uint32_be_unchecked (reader);
  matrix[4] = gst_byte_reader_get_uint32_be_unchecked (reader);
  matrix[5] = gst_byte_reader_get_uint32_be_unchecked (reader);
  matrix[6] = gst_byte_reader_get_uint32_be_unchecked (reader);
  matrix[7] = gst_byte_reader_get_uint32_be_unchecked (reader);
  matrix[8] = gst_byte_reader_get_uint32_be_unchecked (reader);

  GST_DEBUG_OBJECT (qtdemux, "Transformation matrix from atom %s", atom);
  GST_DEBUG_OBJECT (qtdemux, "%u.%u %u.%u %u.%u", matrix[0] >> 16,
      matrix[0] & 0xFFFF, matrix[1] >> 16, matrix[1] & 0xFF, matrix[2] >> 16,
      matrix[2] & 0xFF);
  GST_DEBUG_OBJECT (qtdemux, "%u.%u %u.%u %u.%u", matrix[3] >> 16,
      matrix[3] & 0xFFFF, matrix[4] >> 16, matrix[4] & 0xFF, matrix[5] >> 16,
      matrix[5] & 0xFF);
  GST_DEBUG_OBJECT (qtdemux, "%u.%u %u.%u %u.%u", matrix[6] >> 16,
      matrix[6] & 0xFFFF, matrix[7] >> 16, matrix[7] & 0xFF, matrix[8] >> 16,
      matrix[8] & 0xFF);

  return TRUE;
}