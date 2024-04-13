gst_matroska_demux_loop (GstPad * pad)
{
  GstMatroskaDemux *demux = GST_MATROSKA_DEMUX (GST_PAD_PARENT (pad));
  GstFlowReturn ret;
  guint32 id;
  guint64 length;
  guint needed;

  /* If we have to close a segment, send a new segment to do this now */
  if (G_LIKELY (demux->common.state == GST_MATROSKA_READ_STATE_DATA)) {
    if (G_UNLIKELY (demux->new_segment)) {
      gst_matroska_demux_send_event (demux, demux->new_segment);
      demux->new_segment = NULL;
    }
  }

  ret = gst_matroska_read_common_peek_id_length_pull (&demux->common,
      GST_ELEMENT_CAST (demux), &id, &length, &needed);
  if (ret == GST_FLOW_EOS) {
    goto eos;
  } else if (ret == GST_FLOW_FLUSHING) {
    goto pause;
  } else if (ret != GST_FLOW_OK) {
    ret = gst_matroska_demux_check_parse_error (demux);

    /* Only handle EOS as no error if we're outside the segment already */
    if (ret == GST_FLOW_EOS && (demux->common.ebml_segment_length != G_MAXUINT64
            && demux->common.offset >=
            demux->common.ebml_segment_start +
            demux->common.ebml_segment_length))
      goto eos;
    else if (ret != GST_FLOW_OK)
      goto pause;
    else
      return;
  }

  GST_LOG_OBJECT (demux, "Offset %" G_GUINT64_FORMAT ", Element id 0x%x, "
      "size %" G_GUINT64_FORMAT ", needed %d", demux->common.offset, id,
      length, needed);

  ret = gst_matroska_demux_parse_id (demux, id, length, needed);
  if (ret == GST_FLOW_EOS)
    goto eos;
  if (ret != GST_FLOW_OK)
    goto pause;

  /* check if we're at the end of a configured segment */
  if (G_LIKELY (demux->common.src->len)) {
    guint i;

    g_assert (demux->common.num_streams == demux->common.src->len);
    for (i = 0; i < demux->common.src->len; i++) {
      GstMatroskaTrackContext *context = g_ptr_array_index (demux->common.src,
          i);
      GST_DEBUG_OBJECT (context->pad, "pos %" GST_TIME_FORMAT,
          GST_TIME_ARGS (context->pos));
      if (context->eos == FALSE)
        goto next;
    }

    GST_INFO_OBJECT (demux, "All streams are EOS");
    ret = GST_FLOW_EOS;
    goto eos;
  }

next:
  if (G_UNLIKELY (demux->cached_length == G_MAXUINT64 ||
          demux->common.offset >= demux->cached_length)) {
    demux->cached_length = gst_matroska_read_common_get_length (&demux->common);
    if (demux->common.offset == demux->cached_length) {
      GST_LOG_OBJECT (demux, "Reached end of stream");
      ret = GST_FLOW_EOS;
      goto eos;
    }
  }

  return;

  /* ERRORS */
eos:
  {
    if (demux->common.segment.rate < 0.0) {
      ret = gst_matroska_demux_seek_to_previous_keyframe (demux);
      if (ret == GST_FLOW_OK)
        return;
    }
    /* fall-through */
  }
pause:
  {
    const gchar *reason = gst_flow_get_name (ret);
    gboolean push_eos = FALSE;

    GST_LOG_OBJECT (demux, "pausing task, reason %s", reason);
    gst_pad_pause_task (demux->common.sinkpad);

    if (ret == GST_FLOW_EOS) {
      /* perform EOS logic */

      /* If we were in the headers, make sure we send no-more-pads.
         This will ensure decodebin does not get stuck thinking
         the chain is not complete yet, and waiting indefinitely. */
      if (G_UNLIKELY (demux->common.state == GST_MATROSKA_READ_STATE_HEADER)) {
        if (demux->common.src->len == 0) {
          GST_ELEMENT_ERROR (demux, STREAM, FAILED, (NULL),
              ("No pads created"));
        } else {
          GST_ELEMENT_WARNING (demux, STREAM, DEMUX, (NULL),
              ("Failed to finish reading headers"));
        }
        gst_element_no_more_pads (GST_ELEMENT (demux));
      }

      if (demux->common.segment.flags & GST_SEEK_FLAG_SEGMENT) {
        GstEvent *event;
        GstMessage *msg;
        gint64 stop;

        /* for segment playback we need to post when (in stream time)
         * we stopped, this is either stop (when set) or the duration. */
        if ((stop = demux->common.segment.stop) == -1)
          stop = demux->last_stop_end;

        GST_LOG_OBJECT (demux, "Sending segment done, at end of segment");
        msg = gst_message_new_segment_done (GST_OBJECT (demux), GST_FORMAT_TIME,
            stop);
        if (demux->segment_seqnum)
          gst_message_set_seqnum (msg, demux->segment_seqnum);
        gst_element_post_message (GST_ELEMENT (demux), msg);

        event = gst_event_new_segment_done (GST_FORMAT_TIME, stop);
        if (demux->segment_seqnum)
          gst_event_set_seqnum (event, demux->segment_seqnum);
        gst_matroska_demux_send_event (demux, event);
      } else {
        push_eos = TRUE;
      }
    } else if (ret == GST_FLOW_NOT_LINKED || ret < GST_FLOW_EOS) {
      /* for fatal errors we post an error message */
      GST_ELEMENT_FLOW_ERROR (demux, ret);
      push_eos = TRUE;
    }
    if (push_eos) {
      GstEvent *event;

      /* send EOS, and prevent hanging if no streams yet */
      GST_LOG_OBJECT (demux, "Sending EOS, at end of stream");
      event = gst_event_new_eos ();
      if (demux->segment_seqnum)
        gst_event_set_seqnum (event, demux->segment_seqnum);
      if (!gst_matroska_demux_send_event (demux, event) &&
          (ret == GST_FLOW_EOS)) {
        GST_ELEMENT_ERROR (demux, STREAM, DEMUX,
            (NULL), ("got eos but no streams (yet)"));
      }
    }
    return;
  }
}