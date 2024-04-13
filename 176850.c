gst_matroska_demux_handle_sink_query (GstPad * pad, GstObject * parent,
    GstQuery * query)
{
  GstMatroskaDemux *demux = GST_MATROSKA_DEMUX (parent);
  gboolean res = FALSE;

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_BITRATE:
    {
      if (G_UNLIKELY (demux->cached_length == G_MAXUINT64 ||
              demux->common.offset >= demux->cached_length)) {
        demux->cached_length =
            gst_matroska_read_common_get_length (&demux->common);
      }

      if (demux->cached_length < G_MAXUINT64
          && demux->common.segment.duration > 0) {
        /* TODO: better results based on ranges/index tables */
        guint bitrate =
            gst_util_uint64_scale (8 * demux->cached_length, GST_SECOND,
            demux->common.segment.duration);

        GST_LOG_OBJECT (demux, "bitrate query byte length: %" G_GUINT64_FORMAT
            " duration %" GST_TIME_FORMAT " resulting in a bitrate of %u",
            demux->cached_length,
            GST_TIME_ARGS (demux->common.segment.duration), bitrate);

        gst_query_set_bitrate (query, bitrate);
        res = TRUE;
      }
      break;
    }
    default:
      res = gst_pad_query_default (pad, (GstObject *) demux, query);
      break;
  }

  return res;
}