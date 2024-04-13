gst_qtdemux_loop (GstPad * pad)
{
  GstQTDemux *qtdemux;
  guint64 cur_offset;
  GstFlowReturn ret;

  qtdemux = GST_QTDEMUX (gst_pad_get_parent (pad));

  cur_offset = qtdemux->offset;
  GST_LOG_OBJECT (qtdemux, "loop at position %" G_GUINT64_FORMAT ", state %s",
      cur_offset, qt_demux_state_string (qtdemux->state));

  switch (qtdemux->state) {
    case QTDEMUX_STATE_INITIAL:
    case QTDEMUX_STATE_HEADER:
      ret = gst_qtdemux_loop_state_header (qtdemux);
      break;
    case QTDEMUX_STATE_MOVIE:
      ret = gst_qtdemux_loop_state_movie (qtdemux);
      if (qtdemux->segment.rate < 0 && ret == GST_FLOW_EOS) {
        ret = gst_qtdemux_seek_to_previous_keyframe (qtdemux);
      }
      break;
    default:
      /* ouch */
      goto invalid_state;
  }

  /* if something went wrong, pause */
  if (ret != GST_FLOW_OK)
    goto pause;

done:
  gst_object_unref (qtdemux);
  return;

  /* ERRORS */
invalid_state:
  {
    GST_ELEMENT_ERROR (qtdemux, STREAM, FAILED,
        (NULL), ("streaming stopped, invalid state"));
    gst_pad_pause_task (pad);
    gst_qtdemux_push_event (qtdemux, gst_event_new_eos ());
    goto done;
  }
pause:
  {
    const gchar *reason = gst_flow_get_name (ret);

    GST_LOG_OBJECT (qtdemux, "pausing task, reason %s", reason);

    gst_pad_pause_task (pad);

    /* fatal errors need special actions */
    /* check EOS */
    if (ret == GST_FLOW_EOS) {
      if (qtdemux->n_streams == 0) {
        /* we have no streams, post an error */
        gst_qtdemux_post_no_playable_stream_error (qtdemux);
      }
      if (qtdemux->segment.flags & GST_SEEK_FLAG_SEGMENT) {
        gint64 stop;

        if ((stop = qtdemux->segment.stop) == -1)
          stop = qtdemux->segment.duration;

        if (qtdemux->segment.rate >= 0) {
          GstMessage *message;
          GstEvent *event;

          GST_LOG_OBJECT (qtdemux, "Sending segment done, at end of segment");
          message = gst_message_new_segment_done (GST_OBJECT_CAST (qtdemux),
              GST_FORMAT_TIME, stop);
          event = gst_event_new_segment_done (GST_FORMAT_TIME, stop);
          if (qtdemux->segment_seqnum) {
            gst_message_set_seqnum (message, qtdemux->segment_seqnum);
            gst_event_set_seqnum (event, qtdemux->segment_seqnum);
          }
          gst_element_post_message (GST_ELEMENT_CAST (qtdemux), message);
          gst_qtdemux_push_event (qtdemux, event);
        } else {
          GstMessage *message;
          GstEvent *event;

          /*  For Reverse Playback */
          GST_LOG_OBJECT (qtdemux, "Sending segment done, at start of segment");
          message = gst_message_new_segment_done (GST_OBJECT_CAST (qtdemux),
              GST_FORMAT_TIME, qtdemux->segment.start);
          event = gst_event_new_segment_done (GST_FORMAT_TIME,
              qtdemux->segment.start);
          if (qtdemux->segment_seqnum) {
            gst_message_set_seqnum (message, qtdemux->segment_seqnum);
            gst_event_set_seqnum (event, qtdemux->segment_seqnum);
          }
          gst_element_post_message (GST_ELEMENT_CAST (qtdemux), message);
          gst_qtdemux_push_event (qtdemux, event);
        }
      } else {
        GstEvent *event;

        GST_LOG_OBJECT (qtdemux, "Sending EOS at end of segment");
        event = gst_event_new_eos ();
        if (qtdemux->segment_seqnum)
          gst_event_set_seqnum (event, qtdemux->segment_seqnum);
        gst_qtdemux_push_event (qtdemux, event);
      }
    } else if (ret == GST_FLOW_NOT_LINKED || ret < GST_FLOW_EOS) {
      GST_ELEMENT_FLOW_ERROR (qtdemux, ret);
      gst_qtdemux_push_event (qtdemux, gst_event_new_eos ());
    }
    goto done;
  }
}