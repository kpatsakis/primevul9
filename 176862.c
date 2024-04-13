gst_matroska_demux_query (GstMatroskaDemux * demux, GstPad * pad,
    GstQuery * query)
{
  gboolean res = FALSE;
  GstMatroskaTrackContext *context = NULL;

  if (pad) {
    context = gst_pad_get_element_private (pad);
  }

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_POSITION:
    {
      GstFormat format;

      gst_query_parse_position (query, &format, NULL);

      res = TRUE;
      if (format == GST_FORMAT_TIME) {
        GST_OBJECT_LOCK (demux);
        if (context)
          gst_query_set_position (query, GST_FORMAT_TIME,
              MAX (context->pos, demux->stream_start_time) -
              demux->stream_start_time);
        else
          gst_query_set_position (query, GST_FORMAT_TIME,
              MAX (demux->common.segment.position, demux->stream_start_time) -
              demux->stream_start_time);
        GST_OBJECT_UNLOCK (demux);
      } else if (format == GST_FORMAT_DEFAULT && context
          && context->default_duration) {
        GST_OBJECT_LOCK (demux);
        gst_query_set_position (query, GST_FORMAT_DEFAULT,
            context->pos / context->default_duration);
        GST_OBJECT_UNLOCK (demux);
      } else {
        GST_DEBUG_OBJECT (demux,
            "only position query in TIME and DEFAULT format is supported");
        res = FALSE;
      }

      break;
    }
    case GST_QUERY_DURATION:
    {
      GstFormat format;

      gst_query_parse_duration (query, &format, NULL);

      res = TRUE;
      if (format == GST_FORMAT_TIME) {
        GST_OBJECT_LOCK (demux);
        gst_query_set_duration (query, GST_FORMAT_TIME,
            demux->common.segment.duration);
        GST_OBJECT_UNLOCK (demux);
      } else if (format == GST_FORMAT_DEFAULT && context
          && context->default_duration) {
        GST_OBJECT_LOCK (demux);
        gst_query_set_duration (query, GST_FORMAT_DEFAULT,
            demux->common.segment.duration / context->default_duration);
        GST_OBJECT_UNLOCK (demux);
      } else {
        GST_DEBUG_OBJECT (demux,
            "only duration query in TIME and DEFAULT format is supported");
        res = FALSE;
      }
      break;
    }

    case GST_QUERY_SEEKING:
    {
      GstFormat fmt;

      gst_query_parse_seeking (query, &fmt, NULL, NULL, NULL);
      GST_OBJECT_LOCK (demux);
      if (fmt == GST_FORMAT_TIME) {
        gboolean seekable;

        if (demux->streaming) {
          /* assuming we'll be able to get an index ... */
          seekable = demux->seekable;
        } else {
          seekable = TRUE;
        }

        gst_query_set_seeking (query, GST_FORMAT_TIME, seekable,
            0, demux->common.segment.duration);
        res = TRUE;
      }
      GST_OBJECT_UNLOCK (demux);
      break;
    }
    case GST_QUERY_SEGMENT:
    {
      GstFormat format;
      gint64 start, stop;

      format = demux->common.segment.format;

      start =
          gst_segment_to_stream_time (&demux->common.segment, format,
          demux->common.segment.start);
      if ((stop = demux->common.segment.stop) == -1)
        stop = demux->common.segment.duration;
      else
        stop =
            gst_segment_to_stream_time (&demux->common.segment, format, stop);

      gst_query_set_segment (query, demux->common.segment.rate, format, start,
          stop);
      res = TRUE;
      break;
    }
    default:
      if (pad)
        res = gst_pad_query_default (pad, (GstObject *) demux, query);
      else
        res =
            GST_ELEMENT_CLASS (parent_class)->query (GST_ELEMENT_CAST (demux),
            query);
      break;
  }

  return res;
}