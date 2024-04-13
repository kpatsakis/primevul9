gst_aac_parse_get_audio_object_type (GstAacParse * aacparse, GstBitReader * br,
    guint8 * audio_object_type)
{
  if (!gst_bit_reader_get_bits_uint8 (br, audio_object_type, 5))
    return FALSE;
  if (*audio_object_type == 31) {
    if (!gst_bit_reader_get_bits_uint8 (br, audio_object_type, 6))
      return FALSE;
    *audio_object_type += 32;
  }
  GST_LOG_OBJECT (aacparse, "audio object type %u", *audio_object_type);
  return TRUE;
}