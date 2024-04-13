qtdemux_parse_tfhd (GstQTDemux * qtdemux, GstByteReader * tfhd,
    QtDemuxStream ** stream, guint32 * default_sample_duration,
    guint32 * default_sample_size, guint32 * default_sample_flags,
    gint64 * base_offset)
{
  guint32 flags = 0;
  guint32 track_id = 0;

  if (!gst_byte_reader_skip (tfhd, 1) ||
      !gst_byte_reader_get_uint24_be (tfhd, &flags))
    goto invalid_track;

  if (!gst_byte_reader_get_uint32_be (tfhd, &track_id))
    goto invalid_track;

  *stream = qtdemux_find_stream (qtdemux, track_id);
  if (G_UNLIKELY (!*stream))
    goto unknown_stream;

  if (flags & TF_DEFAULT_BASE_IS_MOOF)
    *base_offset = qtdemux->moof_offset;

  if (flags & TF_BASE_DATA_OFFSET)
    if (!gst_byte_reader_get_uint64_be (tfhd, (guint64 *) base_offset))
      goto invalid_track;

  /* obtain stream defaults */
  qtdemux_parse_trex (qtdemux, *stream,
      default_sample_duration, default_sample_size, default_sample_flags);

  /* FIXME: Handle TF_SAMPLE_DESCRIPTION_INDEX properly */
  if (flags & TF_SAMPLE_DESCRIPTION_INDEX)
    if (!gst_byte_reader_skip (tfhd, 4))
      goto invalid_track;

  if (flags & TF_DEFAULT_SAMPLE_DURATION)
    if (!gst_byte_reader_get_uint32_be (tfhd, default_sample_duration))
      goto invalid_track;

  if (flags & TF_DEFAULT_SAMPLE_SIZE)
    if (!gst_byte_reader_get_uint32_be (tfhd, default_sample_size))
      goto invalid_track;

  if (flags & TF_DEFAULT_SAMPLE_FLAGS)
    if (!gst_byte_reader_get_uint32_be (tfhd, default_sample_flags))
      goto invalid_track;

  return TRUE;

invalid_track:
  {
    GST_WARNING_OBJECT (qtdemux, "invalid track fragment header");
    return FALSE;
  }
unknown_stream:
  {
    GST_DEBUG_OBJECT (qtdemux, "unknown stream in tfhd");
    return TRUE;
  }
}