gst_matroska_demux_add_mpeg_seq_header (GstElement * element,
    GstMatroskaTrackContext * stream, GstBuffer ** buf)
{
  guint8 *seq_header;
  guint seq_header_len;
  guint32 header, tmp;

  if (stream->codec_state) {
    seq_header = stream->codec_state;
    seq_header_len = stream->codec_state_size;
  } else if (stream->codec_priv) {
    seq_header = stream->codec_priv;
    seq_header_len = stream->codec_priv_size;
  } else {
    return GST_FLOW_OK;
  }

  /* Sequence header only needed for keyframes */
  if (GST_BUFFER_FLAG_IS_SET (*buf, GST_BUFFER_FLAG_DELTA_UNIT))
    return GST_FLOW_OK;

  if (gst_buffer_get_size (*buf) < 4)
    return GST_FLOW_OK;

  gst_buffer_extract (*buf, 0, &tmp, sizeof (guint32));
  header = GUINT32_FROM_BE (tmp);

  /* Sequence start code, if not found prepend */
  if (header != 0x000001b3) {
    GstBuffer *newbuf;

    GST_DEBUG_OBJECT (element, "Prepending MPEG sequence header");

    newbuf = gst_buffer_new_wrapped (g_memdup (seq_header, seq_header_len),
        seq_header_len);

    gst_buffer_copy_into (newbuf, *buf, GST_BUFFER_COPY_TIMESTAMPS |
        GST_BUFFER_COPY_FLAGS | GST_BUFFER_COPY_MEMORY, 0,
        gst_buffer_get_size (*buf));

    gst_buffer_unref (*buf);
    *buf = newbuf;
  }

  return GST_FLOW_OK;
}