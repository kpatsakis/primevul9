gst_matroska_demux_handle_seek_event (GstMatroskaDemux * demux,
    GstPad * pad, GstEvent * event)
{
  GstMatroskaIndex *entry = NULL;
  GstMatroskaIndex scan_entry;
  GstSeekFlags flags;
  GstSeekType cur_type, stop_type;
  GstFormat format;
  gboolean flush, keyunit, instant_rate_change, before, after, accurate,
      snap_next;
  gdouble rate;
  gint64 cur, stop;
  GstMatroskaTrackContext *track = NULL;
  GstSegment seeksegment = { 0, };
  guint64 seekpos;
  gboolean update = TRUE;
  gboolean pad_locked = FALSE;
  guint32 seqnum;
  GstSearchMode snap_dir;

  g_return_val_if_fail (event != NULL, FALSE);

  if (pad)
    track = gst_pad_get_element_private (pad);

  GST_DEBUG_OBJECT (demux, "Have seek %" GST_PTR_FORMAT, event);

  gst_event_parse_seek (event, &rate, &format, &flags, &cur_type, &cur,
      &stop_type, &stop);
  seqnum = gst_event_get_seqnum (event);

  /* we can only seek on time */
  if (format != GST_FORMAT_TIME) {
    GST_DEBUG_OBJECT (demux, "Can only seek on TIME");
    return FALSE;
  }

  GST_DEBUG_OBJECT (demux, "configuring seek");

  flush = ! !(flags & GST_SEEK_FLAG_FLUSH);
  keyunit = ! !(flags & GST_SEEK_FLAG_KEY_UNIT);
  after = ! !(flags & GST_SEEK_FLAG_SNAP_AFTER);
  before = ! !(flags & GST_SEEK_FLAG_SNAP_BEFORE);
  accurate = ! !(flags & GST_SEEK_FLAG_ACCURATE);
  instant_rate_change = ! !(flags & GST_SEEK_FLAG_INSTANT_RATE_CHANGE);

  /* Directly send the instant-rate-change event here before taking the
   * stream-lock so that it can be applied as soon as possible */
  if (instant_rate_change) {
    GstEvent *ev;

    /* instant rate change only supported if direction does not change. All
     * other requirements are already checked before creating the seek event
     * but let's double-check here to be sure */
    if ((rate > 0 && demux->common.segment.rate < 0) ||
        (rate < 0 && demux->common.segment.rate > 0) ||
        cur_type != GST_SEEK_TYPE_NONE ||
        stop_type != GST_SEEK_TYPE_NONE || flush) {
      GST_ERROR_OBJECT (demux,
          "Instant rate change seeks only supported in the "
          "same direction, without flushing and position change");
      return FALSE;
    }

    ev = gst_event_new_instant_rate_change (rate /
        demux->common.segment.rate, (GstSegmentFlags) flags);
    gst_event_set_seqnum (ev, seqnum);
    gst_matroska_demux_send_event (demux, ev);
    return TRUE;
  }

  /* copy segment, we need this because we still need the old
   * segment when we close the current segment. */
  memcpy (&seeksegment, &demux->common.segment, sizeof (GstSegment));

  /* pull mode without index means that the actual duration is not known,
   * we might be playing a file that's still being recorded
   * so, invalidate our current duration, which is only a moving target,
   * and should not be used to clamp anything */
  if (!demux->streaming && !demux->common.index && demux->invalid_duration) {
    seeksegment.duration = GST_CLOCK_TIME_NONE;
  }

  /* Subtract stream_start_time so we always seek on a segment
   * in stream time */
  if (GST_CLOCK_TIME_IS_VALID (demux->stream_start_time)) {
    seeksegment.start -= demux->stream_start_time;
    seeksegment.position -= demux->stream_start_time;
    if (GST_CLOCK_TIME_IS_VALID (seeksegment.stop))
      seeksegment.stop -= demux->stream_start_time;
    else
      seeksegment.stop = seeksegment.duration;
  }

  if (!gst_segment_do_seek (&seeksegment, rate, format, flags,
          cur_type, cur, stop_type, stop, &update)) {
    GST_WARNING_OBJECT (demux, "gst_segment_do_seek() failed.");
    return FALSE;
  }

  /* Restore the clip timestamp offset */
  if (GST_CLOCK_TIME_IS_VALID (demux->stream_start_time)) {
    seeksegment.position += demux->stream_start_time;
    seeksegment.start += demux->stream_start_time;
    if (!GST_CLOCK_TIME_IS_VALID (seeksegment.stop))
      seeksegment.stop = seeksegment.duration;
    if (GST_CLOCK_TIME_IS_VALID (seeksegment.stop))
      seeksegment.stop += demux->stream_start_time;
  }

  /* restore segment duration (if any effect),
   * would be determined again when parsing, but anyway ... */
  seeksegment.duration = demux->common.segment.duration;

  /* always do full update if flushing,
   * otherwise problems might arise downstream with missing keyframes etc */
  update = update || flush;

  GST_DEBUG_OBJECT (demux, "New segment %" GST_SEGMENT_FORMAT, &seeksegment);

  /* check sanity before we start flushing and all that */
  snap_next = after && !before;
  if (seeksegment.rate < 0)
    snap_dir = snap_next ? GST_SEARCH_MODE_BEFORE : GST_SEARCH_MODE_AFTER;
  else
    snap_dir = snap_next ? GST_SEARCH_MODE_AFTER : GST_SEARCH_MODE_BEFORE;

  GST_OBJECT_LOCK (demux);

  seekpos = seeksegment.position;
  if (accurate) {
    seekpos -= MIN (seeksegment.position, demux->audio_lead_in_ts);
  }

  track = gst_matroska_read_common_get_seek_track (&demux->common, track);
  if ((entry = gst_matroska_read_common_do_index_seek (&demux->common, track,
              seekpos, &demux->seek_index, &demux->seek_entry,
              snap_dir)) == NULL) {
    /* pull mode without index can scan later on */
    if (demux->streaming) {
      GST_DEBUG_OBJECT (demux, "No matching seek entry in index");
      GST_OBJECT_UNLOCK (demux);
      return FALSE;
    } else if (rate < 0.0) {
      /* FIXME: We should build an index during playback or when scanning
       * that can be used here. The reverse playback code requires seek_index
       * and seek_entry to be set!
       */
      GST_DEBUG_OBJECT (demux,
          "No matching seek entry in index, needed for reverse playback");
      GST_OBJECT_UNLOCK (demux);
      return FALSE;
    }
  }
  GST_DEBUG_OBJECT (demux, "Seek position looks sane");
  GST_OBJECT_UNLOCK (demux);

  if (!update) {
    /* only have to update some segment,
     * but also still have to honour flush and so on */
    GST_DEBUG_OBJECT (demux, "... no update");
    /* bad goto, bad ... */
    goto next;
  }

  if (demux->streaming)
    goto finish;

next:
  if (flush) {
    GstEvent *flush_event = gst_event_new_flush_start ();
    gst_event_set_seqnum (flush_event, seqnum);
    GST_DEBUG_OBJECT (demux, "Starting flush");
    gst_pad_push_event (demux->common.sinkpad, gst_event_ref (flush_event));
    gst_matroska_demux_send_event (demux, flush_event);
  } else {
    GST_DEBUG_OBJECT (demux, "Non-flushing seek, pausing task");
    gst_pad_pause_task (demux->common.sinkpad);
  }
  /* ouch */
  if (!update) {
    GST_PAD_STREAM_LOCK (demux->common.sinkpad);
    pad_locked = TRUE;
    goto exit;
  }

  /* now grab the stream lock so that streaming cannot continue, for
   * non flushing seeks when the element is in PAUSED this could block
   * forever. */
  GST_DEBUG_OBJECT (demux, "Waiting for streaming to stop");
  GST_PAD_STREAM_LOCK (demux->common.sinkpad);
  pad_locked = TRUE;

  /* pull mode without index can do some scanning */
  if (!demux->streaming && !entry) {
    GstEvent *flush_event;

    /* need to stop flushing upstream as we need it next */
    if (flush) {
      flush_event = gst_event_new_flush_stop (TRUE);
      gst_event_set_seqnum (flush_event, seqnum);
      gst_pad_push_event (demux->common.sinkpad, flush_event);
    }
    entry = gst_matroska_demux_search_pos (demux, seekpos);
    /* keep local copy */
    if (entry) {
      scan_entry = *entry;
      g_free (entry);
      entry = &scan_entry;
    } else {
      GST_DEBUG_OBJECT (demux, "Scan failed to find matching position");
      if (flush) {
        flush_event = gst_event_new_flush_stop (TRUE);
        gst_event_set_seqnum (flush_event, seqnum);
        gst_matroska_demux_send_event (demux, flush_event);
      }
      goto seek_error;
    }
  }

finish:
  if (keyunit && seeksegment.rate > 0) {
    GST_DEBUG_OBJECT (demux, "seek to key unit, adjusting segment start from %"
        GST_TIME_FORMAT " to %" GST_TIME_FORMAT,
        GST_TIME_ARGS (seeksegment.start), GST_TIME_ARGS (entry->time));
    seeksegment.start = MAX (entry->time, demux->stream_start_time);
    seeksegment.position = seeksegment.start;
    seeksegment.time = seeksegment.start - demux->stream_start_time;
  } else if (keyunit) {
    GST_DEBUG_OBJECT (demux, "seek to key unit, adjusting segment stop from %"
        GST_TIME_FORMAT " to %" GST_TIME_FORMAT,
        GST_TIME_ARGS (seeksegment.stop), GST_TIME_ARGS (entry->time));
    seeksegment.stop = MAX (entry->time, demux->stream_start_time);
    seeksegment.position = seeksegment.stop;
  }

  if (demux->streaming) {
    GST_OBJECT_LOCK (demux);
    /* track real position we should start at */
    GST_DEBUG_OBJECT (demux, "storing segment start");
    demux->requested_seek_time = seeksegment.position;
    demux->seek_offset = entry->pos + demux->common.ebml_segment_start;
    GST_OBJECT_UNLOCK (demux);
    /* need to seek to cluster start to pick up cluster time */
    /* upstream takes care of flushing and all that
     * ... and newsegment event handling takes care of the rest */
    return perform_seek_to_offset (demux, rate,
        entry->pos + demux->common.ebml_segment_start, seqnum, flags);
  }

exit:
  if (flush) {
    GstEvent *flush_event = gst_event_new_flush_stop (TRUE);
    gst_event_set_seqnum (flush_event, seqnum);
    GST_DEBUG_OBJECT (demux, "Stopping flush");
    gst_pad_push_event (demux->common.sinkpad, gst_event_ref (flush_event));
    gst_matroska_demux_send_event (demux, flush_event);
  }

  GST_OBJECT_LOCK (demux);
  /* now update the real segment info */
  GST_DEBUG_OBJECT (demux, "Committing new seek segment");
  memcpy (&demux->common.segment, &seeksegment, sizeof (GstSegment));
  GST_OBJECT_UNLOCK (demux);

  /* update some (segment) state */
  if (!gst_matroska_demux_move_to_entry (demux, entry, TRUE, update))
    goto seek_error;

  /* notify start of new segment */
  if (demux->common.segment.flags & GST_SEEK_FLAG_SEGMENT) {
    GstMessage *msg;

    msg = gst_message_new_segment_start (GST_OBJECT (demux),
        GST_FORMAT_TIME, demux->common.segment.start);
    gst_message_set_seqnum (msg, seqnum);
    gst_element_post_message (GST_ELEMENT (demux), msg);
  }

  GST_OBJECT_LOCK (demux);
  if (demux->new_segment)
    gst_event_unref (demux->new_segment);

  /* On port from 0.10, discarded !update (for segment.update) here, FIXME? */
  demux->new_segment = gst_event_new_segment (&demux->common.segment);
  gst_event_set_seqnum (demux->new_segment, seqnum);
  if (demux->common.segment.rate < 0 && demux->common.segment.stop == -1)
    demux->to_time = demux->common.segment.position;
  else
    demux->to_time = GST_CLOCK_TIME_NONE;
  demux->segment_seqnum = seqnum;
  GST_OBJECT_UNLOCK (demux);

  /* restart our task since it might have been stopped when we did the
   * flush. */
  gst_pad_start_task (demux->common.sinkpad,
      (GstTaskFunction) gst_matroska_demux_loop, demux->common.sinkpad, NULL);

  /* streaming can continue now */
  if (pad_locked) {
    GST_PAD_STREAM_UNLOCK (demux->common.sinkpad);
  }

  return TRUE;

seek_error:
  {
    if (pad_locked) {
      GST_PAD_STREAM_UNLOCK (demux->common.sinkpad);
    }
    GST_ELEMENT_ERROR (demux, STREAM, DEMUX, (NULL), ("Got a seek error"));
    return FALSE;
  }
}