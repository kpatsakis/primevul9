gst_qtdemux_find_index_for_given_media_offset_linear (GstQTDemux * qtdemux,
    QtDemuxStream * str, gint64 media_offset)
{
  QtDemuxSample *result = str->samples;
  guint32 index = 0;

  if (result == NULL || str->n_samples == 0)
    return -1;

  if (media_offset == result->offset)
    return index;

  result++;
  while (index < str->n_samples - 1) {
    if (!qtdemux_parse_samples (qtdemux, str, index + 1))
      goto parse_failed;

    if (media_offset < result->offset)
      break;

    index++;
    result++;
  }
  return index;

  /* ERRORS */
parse_failed:
  {
    GST_LOG_OBJECT (qtdemux, "Parsing of index %u failed!", index + 1);
    return -1;
  }
}