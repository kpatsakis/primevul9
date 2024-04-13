qtdemux_parse_saio (GstQTDemux * qtdemux, QtDemuxStream * stream,
    GstByteReader * br, guint32 * info_type, guint32 * info_type_parameter,
    guint64 * offset)
{
  guint8 version = 0;
  guint32 flags = 0;
  guint32 aux_info_type = 0;
  guint32 aux_info_type_parameter = 0;
  guint32 entry_count;
  guint32 off_32;
  guint64 off_64;
  const guint8 *aux_info_type_data = NULL;

  g_return_val_if_fail (qtdemux != NULL, FALSE);
  g_return_val_if_fail (stream != NULL, FALSE);
  g_return_val_if_fail (br != NULL, FALSE);
  g_return_val_if_fail (offset != NULL, FALSE);

  if (!gst_byte_reader_get_uint8 (br, &version))
    return FALSE;

  if (!gst_byte_reader_get_uint24_be (br, &flags))
    return FALSE;

  if (flags & 0x1) {

    if (!gst_byte_reader_get_data (br, 4, &aux_info_type_data))
      return FALSE;
    aux_info_type = QT_FOURCC (aux_info_type_data);

    if (!gst_byte_reader_get_uint32_be (br, &aux_info_type_parameter))
      return FALSE;
  } else if (stream->protected) {
    aux_info_type = stream->protection_scheme_type;
  } else {
    aux_info_type = stream->fourcc;
  }

  if (info_type)
    *info_type = aux_info_type;
  if (info_type_parameter)
    *info_type_parameter = aux_info_type_parameter;

  GST_DEBUG_OBJECT (qtdemux, "aux_info_type: '%" GST_FOURCC_FORMAT "', "
      "aux_info_type_parameter:  %#06x",
      GST_FOURCC_ARGS (aux_info_type), aux_info_type_parameter);

  if (!gst_byte_reader_get_uint32_be (br, &entry_count))
    return FALSE;

  if (entry_count != 1) {
    GST_ERROR_OBJECT (qtdemux, "multiple offsets are not supported");
    return FALSE;
  }

  if (version == 0) {
    if (!gst_byte_reader_get_uint32_be (br, &off_32))
      return FALSE;
    *offset = (guint64) off_32;
  } else {
    if (!gst_byte_reader_get_uint64_be (br, &off_64))
      return FALSE;
    *offset = off_64;
  }

  GST_DEBUG_OBJECT (qtdemux, "offset: %" G_GUINT64_FORMAT, *offset);
  return TRUE;
}