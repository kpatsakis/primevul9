qtdemux_parse_trex (GstQTDemux * qtdemux, QtDemuxStream * stream,
    guint32 * ds_duration, guint32 * ds_size, guint32 * ds_flags)
{
  if (!stream->parsed_trex && qtdemux->moov_node) {
    GNode *mvex, *trex;
    GstByteReader trex_data;

    mvex = qtdemux_tree_get_child_by_type (qtdemux->moov_node, FOURCC_mvex);
    if (mvex) {
      trex = qtdemux_tree_get_child_by_type_full (mvex, FOURCC_trex,
          &trex_data);
      while (trex) {
        guint32 id = 0, dur = 0, size = 0, flags = 0, dummy = 0;

        /* skip version/flags */
        if (!gst_byte_reader_skip (&trex_data, 4))
          goto next;
        if (!gst_byte_reader_get_uint32_be (&trex_data, &id))
          goto next;
        if (id != stream->track_id)
          goto next;
        /* sample description index; ignore */
        if (!gst_byte_reader_get_uint32_be (&trex_data, &dummy))
          goto next;
        if (!gst_byte_reader_get_uint32_be (&trex_data, &dur))
          goto next;
        if (!gst_byte_reader_get_uint32_be (&trex_data, &size))
          goto next;
        if (!gst_byte_reader_get_uint32_be (&trex_data, &flags))
          goto next;

        GST_DEBUG_OBJECT (qtdemux, "fragment defaults for stream %d; "
            "duration %d,  size %d, flags 0x%x", stream->track_id,
            dur, size, flags);

        stream->parsed_trex = TRUE;
        stream->def_sample_duration = dur;
        stream->def_sample_size = size;
        stream->def_sample_flags = flags;

      next:
        /* iterate all siblings */
        trex = qtdemux_tree_get_sibling_by_type_full (trex, FOURCC_trex,
            &trex_data);
      }
    }
  }

  *ds_duration = stream->def_sample_duration;
  *ds_size = stream->def_sample_size;
  *ds_flags = stream->def_sample_flags;

  /* even then, above values are better than random ... */
  if (G_UNLIKELY (!stream->parsed_trex)) {
    GST_WARNING_OBJECT (qtdemux,
        "failed to find fragment defaults for stream %d", stream->track_id);
    return FALSE;
  }

  return TRUE;
}