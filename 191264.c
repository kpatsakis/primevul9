gst_qtdemux_find_index (GstQTDemux * qtdemux, QtDemuxStream * str,
    guint64 media_time)
{
  QtDemuxSample *result;
  guint32 index;

  /* convert media_time to mov format */
  media_time =
      gst_util_uint64_scale_ceil (media_time, str->timescale, GST_SECOND);

  result = gst_util_array_binary_search (str->samples, str->stbl_index + 1,
      sizeof (QtDemuxSample), (GCompareDataFunc) find_func,
      GST_SEARCH_MODE_BEFORE, &media_time, NULL);

  if (G_LIKELY (result))
    index = result - str->samples;
  else
    index = 0;

  return index;
}