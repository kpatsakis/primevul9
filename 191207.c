has_next_entry (GstQTDemux * demux)
{
  QtDemuxStream *stream;
  int i;

  GST_DEBUG_OBJECT (demux, "Checking if there are samples not played yet");

  for (i = 0; i < demux->n_streams; i++) {
    stream = demux->streams[i];

    if (stream->sample_index == -1) {
      stream->sample_index = 0;
      stream->offset_in_sample = 0;
    }

    if (stream->sample_index >= stream->n_samples) {
      GST_LOG_OBJECT (demux, "stream %d samples exhausted", i);
      continue;
    }
    GST_DEBUG_OBJECT (demux, "Found a sample");
    return TRUE;
  }

  GST_DEBUG_OBJECT (demux, "There wasn't any next sample");
  return FALSE;
}