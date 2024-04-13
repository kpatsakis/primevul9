gst_qtdemux_handle_src_query (GstPad * pad, GstObject * parent,
    GstQuery * query)
{
  gboolean res = FALSE;
  GstQTDemux *qtdemux = GST_QTDEMUX (parent);

  GST_LOG_OBJECT (pad, "%s query", GST_QUERY_TYPE_NAME (query));

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_POSITION:{
      GstFormat fmt;

      gst_query_parse_position (query, &fmt, NULL);
      if (fmt == GST_FORMAT_TIME
          && GST_CLOCK_TIME_IS_VALID (qtdemux->segment.position)) {
        gst_query_set_position (query, GST_FORMAT_TIME,
            qtdemux->segment.position);
        res = TRUE;
      }
    }
      break;
    case GST_QUERY_DURATION:{
      GstFormat fmt;

      gst_query_parse_duration (query, &fmt, NULL);
      if (fmt == GST_FORMAT_TIME) {
        /* First try to query upstream */
        res = gst_pad_query_default (pad, parent, query);
        if (!res) {
          GstClockTime duration;
          if (gst_qtdemux_get_duration (qtdemux, &duration) && duration > 0) {
            gst_query_set_duration (query, GST_FORMAT_TIME, duration);
            res = TRUE;
          }
        }
      }
      break;
    }
    case GST_QUERY_CONVERT:{
      GstFormat src_fmt, dest_fmt;
      gint64 src_value, dest_value = 0;

      gst_query_parse_convert (query, &src_fmt, &src_value, &dest_fmt, NULL);

      res = gst_qtdemux_src_convert (qtdemux, pad,
          src_fmt, src_value, dest_fmt, &dest_value);
      if (res) {
        gst_query_set_convert (query, src_fmt, src_value, dest_fmt, dest_value);
        res = TRUE;
      }
      break;
    }
    case GST_QUERY_FORMATS:
      gst_query_set_formats (query, 2, GST_FORMAT_TIME, GST_FORMAT_BYTES);
      res = TRUE;
      break;
    case GST_QUERY_SEEKING:{
      GstFormat fmt;
      gboolean seekable;

      /* try upstream first */
      res = gst_pad_query_default (pad, parent, query);

      if (!res) {
        gst_query_parse_seeking (query, &fmt, NULL, NULL, NULL);
        if (fmt == GST_FORMAT_TIME) {
          GstClockTime duration;

          gst_qtdemux_get_duration (qtdemux, &duration);
          seekable = TRUE;
          if (!qtdemux->pullbased) {
            GstQuery *q;

            /* we might be able with help from upstream */
            seekable = FALSE;
            q = gst_query_new_seeking (GST_FORMAT_BYTES);
            if (gst_pad_peer_query (qtdemux->sinkpad, q)) {
              gst_query_parse_seeking (q, &fmt, &seekable, NULL, NULL);
              GST_LOG_OBJECT (qtdemux, "upstream BYTE seekable %d", seekable);
            }
            gst_query_unref (q);
          }
          gst_query_set_seeking (query, GST_FORMAT_TIME, seekable, 0, duration);
          res = TRUE;
        }
      }
      break;
    }
    case GST_QUERY_SEGMENT:
    {
      GstFormat format;
      gint64 start, stop;

      format = qtdemux->segment.format;

      start =
          gst_segment_to_stream_time (&qtdemux->segment, format,
          qtdemux->segment.start);
      if ((stop = qtdemux->segment.stop) == -1)
        stop = qtdemux->segment.duration;
      else
        stop = gst_segment_to_stream_time (&qtdemux->segment, format, stop);

      gst_query_set_segment (query, qtdemux->segment.rate, format, start, stop);
      res = TRUE;
      break;
    }
    default:
      res = gst_pad_query_default (pad, parent, query);
      break;
  }

  return res;
}