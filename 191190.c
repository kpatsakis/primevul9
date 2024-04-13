gst_qtdemux_move_stream (GstQTDemux * qtdemux, QtDemuxStream * str,
    guint32 index)
{
  /* no change needed */
  if (index == str->sample_index)
    return;

  GST_DEBUG_OBJECT (qtdemux, "moving to sample %u of %u", index,
      str->n_samples);

  /* position changed, we have a discont */
  str->sample_index = index;
  str->offset_in_sample = 0;
  /* Each time we move in the stream we store the position where we are
   * starting from */
  str->from_sample = index;
  str->discont = TRUE;
}