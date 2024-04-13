gst_matroska_demux_push_codec_data_all (GstMatroskaDemux * demux)
{
  gint stream_nr;

  g_assert (demux->common.num_streams == demux->common.src->len);
  for (stream_nr = 0; stream_nr < demux->common.src->len; stream_nr++) {
    GstMatroskaTrackContext *stream;

    stream = g_ptr_array_index (demux->common.src, stream_nr);

    if (stream->send_stream_headers) {
      if (stream->stream_headers != NULL) {
        gst_matroska_demux_push_stream_headers (demux, stream);
      } else {
        /* FIXME: perhaps we can just disable and skip this stream then */
        GST_ELEMENT_ERROR (demux, STREAM, DECODE, (NULL),
            ("Failed to extract stream headers from codec private data"));
      }
      stream->send_stream_headers = FALSE;
    }

    if (stream->send_dvd_event) {
      gst_matroska_demux_push_dvd_clut_change_event (demux, stream);
      /* FIXME: should we send this event again after (flushing) seek ? */
      stream->send_dvd_event = FALSE;
    }
  }

}