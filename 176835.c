gst_matroska_demux_handle_seek_push (GstMatroskaDemux * demux, GstPad * pad,
    GstEvent * event)
{
  GstSeekFlags flags;
  GstSeekType cur_type, stop_type;
  GstFormat format;
  gdouble rate;
  gint64 cur, stop;

  gst_event_parse_seek (event, &rate, &format, &flags, &cur_type, &cur,
      &stop_type, &stop);

  /* Directly send the instant-rate-change event here before taking the
   * stream-lock so that it can be applied as soon as possible */
  if (flags & GST_SEEK_FLAG_INSTANT_RATE_CHANGE) {
    guint32 seqnum;
    GstEvent *ev;

    /* instant rate change only supported if direction does not change. All
     * other requirements are already checked before creating the seek event
     * but let's double-check here to be sure */
    if ((rate > 0 && demux->common.segment.rate < 0) ||
        (rate < 0 && demux->common.segment.rate > 0) ||
        cur_type != GST_SEEK_TYPE_NONE ||
        stop_type != GST_SEEK_TYPE_NONE || (flags & GST_SEEK_FLAG_FLUSH)) {
      GST_ERROR_OBJECT (demux,
          "Instant rate change seeks only supported in the "
          "same direction, without flushing and position change");
      return FALSE;
    }

    seqnum = gst_event_get_seqnum (event);
    ev = gst_event_new_instant_rate_change (rate / demux->common.segment.rate,
        (GstSegmentFlags) flags);
    gst_event_set_seqnum (ev, seqnum);
    gst_matroska_demux_send_event (demux, ev);
    return TRUE;
  }



  /* sanity checks */

  /* we can only seek on time */
  if (format != GST_FORMAT_TIME) {
    GST_DEBUG_OBJECT (demux, "Can only seek on TIME");
    return FALSE;
  }

  if (stop_type != GST_SEEK_TYPE_NONE && stop != GST_CLOCK_TIME_NONE) {
    GST_DEBUG_OBJECT (demux, "Seek end-time not supported in streaming mode");
    return FALSE;
  }

  if (!(flags & GST_SEEK_FLAG_FLUSH)) {
    GST_DEBUG_OBJECT (demux,
        "Non-flushing seek not supported in streaming mode");
    return FALSE;
  }

  if (flags & GST_SEEK_FLAG_SEGMENT) {
    GST_DEBUG_OBJECT (demux, "Segment seek not supported in streaming mode");
    return FALSE;
  }

  /* check for having parsed index already */
  if (!demux->common.index_parsed) {
    gboolean building_index;
    guint64 offset = 0;

    if (!demux->index_offset) {
      GST_DEBUG_OBJECT (demux, "no index (location); no seek in push mode");
      return FALSE;
    }

    GST_OBJECT_LOCK (demux);
    /* handle the seek event in the chain function */
    demux->common.state = GST_MATROSKA_READ_STATE_SEEK;
    /* no more seek can be issued until state reset to _DATA */

    /* copy the event */
    if (demux->seek_event)
      gst_event_unref (demux->seek_event);
    demux->seek_event = gst_event_ref (event);

    /* set the building_index flag so that only one thread can setup the
     * structures for index seeking. */
    building_index = demux->building_index;
    if (!building_index) {
      demux->building_index = TRUE;
      offset = demux->index_offset;
    }
    GST_OBJECT_UNLOCK (demux);

    if (!building_index) {
      /* seek to the first subindex or legacy index */
      GST_INFO_OBJECT (demux, "Seeking to Cues at %" G_GUINT64_FORMAT, offset);
      return perform_seek_to_offset (demux, rate, offset,
          gst_event_get_seqnum (event), GST_SEEK_FLAG_NONE);
    }

    /* well, we are handling it already */
    return TRUE;
  }

  /* delegate to tweaked regular seek */
  return gst_matroska_demux_handle_seek_event (demux, pad, event);
}