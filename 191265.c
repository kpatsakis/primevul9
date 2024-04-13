gst_qtdemux_do_push_seek (GstQTDemux * qtdemux, GstPad * pad, GstEvent * event)
{
  gdouble rate;
  GstFormat format;
  GstSeekFlags flags;
  GstSeekType cur_type, stop_type;
  gint64 cur, stop, key_cur;
  gboolean res;
  gint64 byte_cur;
  gint64 original_stop;
  guint32 seqnum;

  GST_DEBUG_OBJECT (qtdemux, "doing push-based seek");

  gst_event_parse_seek (event, &rate, &format, &flags,
      &cur_type, &cur, &stop_type, &stop);
  seqnum = gst_event_get_seqnum (event);

  /* only forward streaming and seeking is possible */
  if (rate <= 0)
    goto unsupported_seek;

  /* convert to TIME if needed and possible */
  if (!gst_qtdemux_convert_seek (pad, &format, cur_type, &cur,
          stop_type, &stop))
    goto no_format;

  /* Upstream seek in bytes will have undefined stop, but qtdemux stores
   * the original stop position to use when upstream pushes the new segment
   * for this seek */
  original_stop = stop;
  stop = -1;

  /* find reasonable corresponding BYTE position,
   * also try to mind about keyframes, since we can not go back a bit for them
   * later on */
  gst_qtdemux_adjust_seek (qtdemux, cur, FALSE, &key_cur, &byte_cur);

  if (byte_cur == -1)
    goto abort_seek;

  GST_DEBUG_OBJECT (qtdemux, "Pushing BYTE seek rate %g, "
      "start %" G_GINT64_FORMAT ", stop %" G_GINT64_FORMAT, rate, byte_cur,
      stop);

  GST_OBJECT_LOCK (qtdemux);
  qtdemux->seek_offset = byte_cur;
  if (!(flags & GST_SEEK_FLAG_KEY_UNIT)) {
    qtdemux->push_seek_start = cur;
  } else {
    qtdemux->push_seek_start = key_cur;
  }

  if (stop_type == GST_SEEK_TYPE_NONE) {
    qtdemux->push_seek_stop = qtdemux->segment.stop;
  } else {
    qtdemux->push_seek_stop = original_stop;
  }
  GST_OBJECT_UNLOCK (qtdemux);

  /* BYTE seek event */
  event = gst_event_new_seek (rate, GST_FORMAT_BYTES, flags, cur_type, byte_cur,
      stop_type, stop);
  gst_event_set_seqnum (event, seqnum);
  res = gst_pad_push_event (qtdemux->sinkpad, event);

  return res;

  /* ERRORS */
abort_seek:
  {
    GST_DEBUG_OBJECT (qtdemux, "could not determine byte position to seek to, "
        "seek aborted.");
    return FALSE;
  }
unsupported_seek:
  {
    GST_DEBUG_OBJECT (qtdemux, "unsupported seek, seek aborted.");
    return FALSE;
  }
no_format:
  {
    GST_DEBUG_OBJECT (qtdemux, "unsupported format given, seek aborted.");
    return FALSE;
  }
}