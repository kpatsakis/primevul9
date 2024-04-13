gst_matroska_demux_push_stream_headers (GstMatroskaDemux * demux,
    GstMatroskaTrackContext * stream)
{
  GstFlowReturn ret = GST_FLOW_OK;
  gint i, num;

  num = gst_buffer_list_length (stream->stream_headers);
  for (i = 0; i < num; ++i) {
    GstBuffer *buf;

    buf = gst_buffer_list_get (stream->stream_headers, i);
    buf = gst_buffer_copy (buf);

    GST_BUFFER_FLAG_SET (buf, GST_BUFFER_FLAG_HEADER);

    if (stream->set_discont) {
      GST_BUFFER_FLAG_SET (buf, GST_BUFFER_FLAG_DISCONT);
      stream->set_discont = FALSE;
    } else {
      GST_BUFFER_FLAG_UNSET (buf, GST_BUFFER_FLAG_DISCONT);
    }

    /* push out all headers in one go and use last flow return */
    ret = gst_pad_push (stream->pad, buf);
  }

  /* don't need these any  longer */
  gst_buffer_list_unref (stream->stream_headers);
  stream->stream_headers = NULL;

  /* combine flows */
  ret = gst_flow_combiner_update_flow (demux->flowcombiner, ret);

  return ret;
}