gst_qtdemux_do_fragmented_seek (GstQTDemux * qtdemux)
{
  const QtDemuxRandomAccessEntry *best_entry = NULL;
  guint i;

  GST_OBJECT_LOCK (qtdemux);

  g_assert (qtdemux->n_streams > 0);

  for (i = 0; i < qtdemux->n_streams; i++) {
    const QtDemuxRandomAccessEntry *entry;
    QtDemuxStream *stream;
    gboolean is_audio_or_video;

    stream = qtdemux->streams[i];

    g_free (stream->samples);
    stream->samples = NULL;
    stream->n_samples = 0;
    stream->stbl_index = -1;    /* no samples have yet been parsed */
    stream->sample_index = -1;

    if (stream->ra_entries == NULL)
      continue;

    if (stream->subtype == FOURCC_vide || stream->subtype == FOURCC_soun)
      is_audio_or_video = TRUE;
    else
      is_audio_or_video = FALSE;

    entry =
        gst_qtdemux_stream_seek_fragment (qtdemux, stream,
        stream->time_position, !is_audio_or_video);

    GST_INFO_OBJECT (stream->pad, "%" GST_TIME_FORMAT " at offset "
        "%" G_GUINT64_FORMAT, GST_TIME_ARGS (entry->ts), entry->moof_offset);

    stream->pending_seek = entry;

    /* decide position to jump to just based on audio/video tracks, not subs */
    if (!is_audio_or_video)
      continue;

    if (best_entry == NULL || entry->moof_offset < best_entry->moof_offset)
      best_entry = entry;
  }

  if (best_entry == NULL) {
    GST_OBJECT_UNLOCK (qtdemux);
    return FALSE;
  }

  GST_INFO_OBJECT (qtdemux, "seek to %" GST_TIME_FORMAT ", best fragment "
      "moof offset: %" G_GUINT64_FORMAT ", ts %" GST_TIME_FORMAT,
      GST_TIME_ARGS (qtdemux->streams[0]->time_position),
      best_entry->moof_offset, GST_TIME_ARGS (best_entry->ts));

  qtdemux->moof_offset = best_entry->moof_offset;

  qtdemux_add_fragmented_samples (qtdemux);

  GST_OBJECT_UNLOCK (qtdemux);
  return TRUE;
}