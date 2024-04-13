gst_qtdemux_stream_flush_samples_data (GstQTDemux * qtdemux,
    QtDemuxStream * stream)
{
  g_free (stream->samples);
  stream->samples = NULL;
  gst_qtdemux_stbl_free (stream);

  /* fragments */
  g_free (stream->ra_entries);
  stream->ra_entries = NULL;
  stream->n_ra_entries = 0;

  stream->sample_index = -1;
  stream->stbl_index = -1;
  stream->n_samples = 0;
  stream->time_position = 0;

  stream->n_samples_moof = 0;
  stream->duration_moof = 0;
  stream->duration_last_moof = 0;
}