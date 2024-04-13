gst_matroska_demux_check_aac (GstElement * element,
    GstMatroskaTrackContext * stream, GstBuffer ** buf)
{
  guint8 data[2];
  guint size;

  gst_buffer_extract (*buf, 0, data, 2);
  size = gst_buffer_get_size (*buf);

  if (size > 2 && data[0] == 0xff && (data[1] >> 4 == 0x0f)) {
    GstStructure *s;

    /* tss, ADTS data, remove codec_data
     * still assume it is at least parsed */
    stream->caps = gst_caps_make_writable (stream->caps);
    s = gst_caps_get_structure (stream->caps, 0);
    g_assert (s);
    gst_structure_remove_field (s, "codec_data");
    gst_pad_set_caps (stream->pad, stream->caps);
    GST_DEBUG_OBJECT (element, "ADTS AAC audio data; removing codec-data, "
        "new caps: %" GST_PTR_FORMAT, stream->caps);
  }

  /* disable subsequent checking */
  stream->postprocess_frame = NULL;

  return GST_FLOW_OK;
}