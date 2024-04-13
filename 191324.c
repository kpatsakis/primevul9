next_entry_size (GstQTDemux * demux)
{
  QtDemuxStream *stream;
  int i;
  int smallidx = -1;
  guint64 smalloffs = (guint64) - 1;
  QtDemuxSample *sample;

  GST_LOG_OBJECT (demux, "Finding entry at offset %" G_GUINT64_FORMAT,
      demux->offset);

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

    if (!qtdemux_parse_samples (demux, stream, stream->sample_index)) {
      GST_LOG_OBJECT (demux, "Parsing of index %u from stbl atom failed!",
          stream->sample_index);
      return -1;
    }

    sample = &stream->samples[stream->sample_index];

    GST_LOG_OBJECT (demux,
        "Checking Stream %d (sample_index:%d / offset:%" G_GUINT64_FORMAT
        " / size:%" G_GUINT32_FORMAT ")", i, stream->sample_index,
        sample->offset, sample->size);

    if (((smalloffs == -1)
            || (sample->offset < smalloffs)) && (sample->size)) {
      smallidx = i;
      smalloffs = sample->offset;
    }
  }

  GST_LOG_OBJECT (demux,
      "stream %d offset %" G_GUINT64_FORMAT " demux->offset :%"
      G_GUINT64_FORMAT, smallidx, smalloffs, demux->offset);

  if (smallidx == -1)
    return -1;

  stream = demux->streams[smallidx];
  sample = &stream->samples[stream->sample_index];

  if (sample->offset >= demux->offset) {
    demux->todrop = sample->offset - demux->offset;
    return sample->size + demux->todrop;
  }

  GST_DEBUG_OBJECT (demux,
      "There wasn't any entry at offset %" G_GUINT64_FORMAT, demux->offset);
  return -1;
}