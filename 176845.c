gst_matroska_demux_find_tracks (GstMatroskaDemux * demux)
{
  guint32 id;
  guint64 before_pos;
  guint64 length;
  guint needed;
  GstFlowReturn ret = GST_FLOW_OK;

  GST_WARNING_OBJECT (demux,
      "Found Cluster element before Tracks, searching Tracks");

  /* remember */
  before_pos = demux->common.offset;

  /* Search Tracks element */
  while (TRUE) {
    ret = gst_matroska_read_common_peek_id_length_pull (&demux->common,
        GST_ELEMENT_CAST (demux), &id, &length, &needed);
    if (ret != GST_FLOW_OK)
      break;

    if (id != GST_MATROSKA_ID_TRACKS) {
      /* we may be skipping large cluster here, so forego size check etc */
      /* ... but we can't skip undefined size; force error */
      if (length == G_MAXUINT64) {
        ret = gst_matroska_demux_check_read_size (demux, length);
        break;
      } else {
        demux->common.offset += needed;
        demux->common.offset += length;
      }
      continue;
    }

    /* will lead to track parsing ... */
    ret = gst_matroska_demux_parse_id (demux, id, length, needed);
    break;
  }

  /* seek back */
  demux->common.offset = before_pos;

  return ret;
}