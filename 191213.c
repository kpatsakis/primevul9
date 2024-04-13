qtdemux_parse_tfra (GstQTDemux * qtdemux, GNode * tfra_node)
{
  QtDemuxStream *stream;
  guint32 ver_flags, track_id, len, num_entries, i;
  guint value_size, traf_size, trun_size, sample_size;
  guint64 time = 0, moof_offset = 0;
#if 0
  GstBuffer *buf = NULL;
  GstFlowReturn ret;
#endif
  GstByteReader tfra;

  gst_byte_reader_init (&tfra, tfra_node->data, QT_UINT32 (tfra_node->data));

  if (!gst_byte_reader_skip (&tfra, 8))
    return FALSE;

  if (!gst_byte_reader_get_uint32_be (&tfra, &ver_flags))
    return FALSE;

  if (!gst_byte_reader_get_uint32_be (&tfra, &track_id)
      || !gst_byte_reader_get_uint32_be (&tfra, &len)
      || !gst_byte_reader_get_uint32_be (&tfra, &num_entries))
    return FALSE;

  GST_DEBUG_OBJECT (qtdemux, "parsing tfra box for track id %u", track_id);

  stream = qtdemux_find_stream (qtdemux, track_id);
  if (stream == NULL)
    goto unknown_trackid;

  value_size = ((ver_flags >> 24) == 1) ? sizeof (guint64) : sizeof (guint32);
  sample_size = (len & 3) + 1;
  trun_size = ((len & 12) >> 2) + 1;
  traf_size = ((len & 48) >> 4) + 1;

  GST_DEBUG_OBJECT (qtdemux, "%u entries, sizes: value %u, traf %u, trun %u, "
      "sample %u", num_entries, value_size, traf_size, trun_size, sample_size);

  if (num_entries == 0)
    goto no_samples;

  if (!qt_atom_parser_has_chunks (&tfra, num_entries,
          value_size + value_size + traf_size + trun_size + sample_size))
    goto corrupt_file;

  g_free (stream->ra_entries);
  stream->ra_entries = g_new (QtDemuxRandomAccessEntry, num_entries);
  stream->n_ra_entries = num_entries;

  for (i = 0; i < num_entries; i++) {
    qt_atom_parser_get_offset (&tfra, value_size, &time);
    qt_atom_parser_get_offset (&tfra, value_size, &moof_offset);
    qt_atom_parser_get_uint_with_size_unchecked (&tfra, traf_size);
    qt_atom_parser_get_uint_with_size_unchecked (&tfra, trun_size);
    qt_atom_parser_get_uint_with_size_unchecked (&tfra, sample_size);

    time = QTSTREAMTIME_TO_GSTTIME (stream, time);

    GST_LOG_OBJECT (qtdemux, "fragment time: %" GST_TIME_FORMAT ", "
        " moof_offset: %" G_GUINT64_FORMAT, GST_TIME_ARGS (time), moof_offset);

    stream->ra_entries[i].ts = time;
    stream->ra_entries[i].moof_offset = moof_offset;

    /* don't want to go through the entire file and read all moofs at startup */
#if 0
    ret = gst_qtdemux_pull_atom (qtdemux, moof_offset, 0, &buf);
    if (ret != GST_FLOW_OK)
      goto corrupt_file;
    qtdemux_parse_moof (qtdemux, GST_BUFFER_DATA (buf), GST_BUFFER_SIZE (buf),
        moof_offset, stream);
    gst_buffer_unref (buf);
#endif
  }

  check_update_duration (qtdemux, time);

  return TRUE;

/* ERRORS */
unknown_trackid:
  {
    GST_WARNING_OBJECT (qtdemux, "Couldn't find stream for track %u", track_id);
    return FALSE;
  }
corrupt_file:
  {
    GST_WARNING_OBJECT (qtdemux, "broken traf box, ignoring");
    return FALSE;
  }
no_samples:
  {
    GST_WARNING_OBJECT (qtdemux, "stream has no samples");
    return FALSE;
  }
}