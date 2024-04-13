gst_qtdemux_stream_seek_fragment (GstQTDemux * qtdemux, QtDemuxStream * stream,
    GstClockTime pos, gboolean after)
{
  QtDemuxRandomAccessEntry *entries = stream->ra_entries;
  guint n_entries = stream->n_ra_entries;
  guint i;

  /* we assume the table is sorted */
  for (i = 0; i < n_entries; ++i) {
    if (entries[i].ts > pos)
      break;
  }

  /* FIXME: maybe save first moof_offset somewhere instead, but for now it's
   * probably okay to assume that the index lists the very first fragment */
  if (i == 0)
    return &entries[0];

  if (after)
    return &entries[i];
  else
    return &entries[i - 1];
}