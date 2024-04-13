gst_qtdemux_find_index_linear (GstQTDemux * qtdemux, QtDemuxStream * str,
    GstClockTime media_time)
{
  guint32 index = 0;
  guint64 mov_time;
  QtDemuxSample *sample;

  /* convert media_time to mov format */
  mov_time =
      gst_util_uint64_scale_ceil (media_time, str->timescale, GST_SECOND);

  sample = str->samples;
  if (mov_time == sample->timestamp + sample->pts_offset)
    return index;

  /* use faster search if requested time in already parsed range */
  sample = str->samples + str->stbl_index;
  if (str->stbl_index >= 0 &&
      mov_time <= (sample->timestamp + sample->pts_offset))
    return gst_qtdemux_find_index (qtdemux, str, media_time);

  while (index < str->n_samples - 1) {
    if (!qtdemux_parse_samples (qtdemux, str, index + 1))
      goto parse_failed;

    sample = str->samples + index + 1;
    if (mov_time < (sample->timestamp + sample->pts_offset))
      break;

    index++;
  }
  return index;

  /* ERRORS */
parse_failed:
  {
    GST_LOG_OBJECT (qtdemux, "Parsing of index %u failed!", index + 1);
    return -1;
  }
}