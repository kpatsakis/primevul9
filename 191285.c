gst_qtdemux_do_seek (GstQTDemux * qtdemux, GstPad * pad, GstEvent * event)
{
  gdouble rate;
  GstFormat format;
  GstSeekFlags flags;
  GstSeekType cur_type, stop_type;
  gint64 cur, stop;
  gboolean flush;
  gboolean update;
  GstSegment seeksegment;
  guint32 seqnum = 0;
  GstEvent *flush_event;

  if (event) {
    GST_DEBUG_OBJECT (qtdemux, "doing seek with event");

    gst_event_parse_seek (event, &rate, &format, &flags,
        &cur_type, &cur, &stop_type, &stop);
    seqnum = gst_event_get_seqnum (event);

    /* we have to have a format as the segment format. Try to convert
     * if not. */
    if (!gst_qtdemux_convert_seek (pad, &format, cur_type, &cur,
            stop_type, &stop))
      goto no_format;

    GST_DEBUG_OBJECT (qtdemux, "seek format %s", gst_format_get_name (format));
  } else {
    GST_DEBUG_OBJECT (qtdemux, "doing seek without event");
    flags = 0;
  }

  flush = flags & GST_SEEK_FLAG_FLUSH;

  /* stop streaming, either by flushing or by pausing the task */
  if (flush) {
    flush_event = gst_event_new_flush_start ();
    if (seqnum)
      gst_event_set_seqnum (flush_event, seqnum);
    /* unlock upstream pull_range */
    gst_pad_push_event (qtdemux->sinkpad, gst_event_ref (flush_event));
    /* make sure out loop function exits */
    gst_qtdemux_push_event (qtdemux, flush_event);
  } else {
    /* non flushing seek, pause the task */
    gst_pad_pause_task (qtdemux->sinkpad);
  }

  /* wait for streaming to finish */
  GST_PAD_STREAM_LOCK (qtdemux->sinkpad);

  /* copy segment, we need this because we still need the old
   * segment when we close the current segment. */
  memcpy (&seeksegment, &qtdemux->segment, sizeof (GstSegment));

  if (event) {
    /* configure the segment with the seek variables */
    GST_DEBUG_OBJECT (qtdemux, "configuring seek");
    gst_segment_do_seek (&seeksegment, rate, format, flags,
        cur_type, cur, stop_type, stop, &update);
  }

  /* now do the seek, this actually never returns FALSE */
  gst_qtdemux_perform_seek (qtdemux, &seeksegment, seqnum, flags);

  /* prepare for streaming again */
  if (flush) {
    flush_event = gst_event_new_flush_stop (TRUE);
    if (seqnum)
      gst_event_set_seqnum (flush_event, seqnum);

    gst_pad_push_event (qtdemux->sinkpad, gst_event_ref (flush_event));
    gst_qtdemux_push_event (qtdemux, flush_event);
  }

  /* commit the new segment */
  memcpy (&qtdemux->segment, &seeksegment, sizeof (GstSegment));

  if (qtdemux->segment.flags & GST_SEEK_FLAG_SEGMENT) {
    GstMessage *msg = gst_message_new_segment_start (GST_OBJECT_CAST (qtdemux),
        qtdemux->segment.format, qtdemux->segment.position);
    if (seqnum)
      gst_message_set_seqnum (msg, seqnum);
    gst_element_post_message (GST_ELEMENT_CAST (qtdemux), msg);
  }

  /* restart streaming, NEWSEGMENT will be sent from the streaming thread. */
  gst_pad_start_task (qtdemux->sinkpad, (GstTaskFunction) gst_qtdemux_loop,
      qtdemux->sinkpad, NULL);

  GST_PAD_STREAM_UNLOCK (qtdemux->sinkpad);

  return TRUE;

  /* ERRORS */
no_format:
  {
    GST_DEBUG_OBJECT (qtdemux, "unsupported format given, seek aborted.");
    return FALSE;
  }
}