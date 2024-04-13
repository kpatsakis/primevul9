gst_qtdemux_check_seekability (GstQTDemux * demux)
{
  GstQuery *query;
  gboolean seekable = FALSE;
  gint64 start = -1, stop = -1;

  if (demux->upstream_size)
    return;

  query = gst_query_new_seeking (GST_FORMAT_BYTES);
  if (!gst_pad_peer_query (demux->sinkpad, query)) {
    GST_DEBUG_OBJECT (demux, "seeking query failed");
    goto done;
  }

  gst_query_parse_seeking (query, NULL, &seekable, &start, &stop);

  /* try harder to query upstream size if we didn't get it the first time */
  if (seekable && stop == -1) {
    GST_DEBUG_OBJECT (demux, "doing duration query to fix up unset stop");
    gst_pad_peer_query_duration (demux->sinkpad, GST_FORMAT_BYTES, &stop);
  }

  /* if upstream doesn't know the size, it's likely that it's not seekable in
   * practice even if it technically may be seekable */
  if (seekable && (start != 0 || stop <= start)) {
    GST_DEBUG_OBJECT (demux, "seekable but unknown start/stop -> disable");
    seekable = FALSE;
  }

done:
  gst_query_unref (query);

  GST_DEBUG_OBJECT (demux, "seekable: %d (%" G_GUINT64_FORMAT " - %"
      G_GUINT64_FORMAT ")", seekable, start, stop);
  demux->upstream_seekable = seekable;
  demux->upstream_size = seekable ? stop : -1;
}