gst_matroska_demux_subtitle_caps (GstMatroskaTrackSubtitleContext *
    subtitlecontext, const gchar * codec_id, gpointer data, guint size)
{
  GstCaps *caps = NULL;
  GstMatroskaTrackContext *context =
      (GstMatroskaTrackContext *) subtitlecontext;

  /* for backwards compatibility */
  if (!g_ascii_strcasecmp (codec_id, GST_MATROSKA_CODEC_ID_SUBTITLE_ASCII))
    codec_id = GST_MATROSKA_CODEC_ID_SUBTITLE_UTF8;
  else if (!g_ascii_strcasecmp (codec_id, "S_SSA"))
    codec_id = GST_MATROSKA_CODEC_ID_SUBTITLE_SSA;
  else if (!g_ascii_strcasecmp (codec_id, "S_ASS"))
    codec_id = GST_MATROSKA_CODEC_ID_SUBTITLE_ASS;
  else if (!g_ascii_strcasecmp (codec_id, "S_USF"))
    codec_id = GST_MATROSKA_CODEC_ID_SUBTITLE_USF;

  /* TODO: Add GST_MATROSKA_CODEC_ID_SUBTITLE_BMP support
   * Check if we have to do something with codec_private */
  if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_SUBTITLE_UTF8)) {
    /* well, plain text simply does not have a lot of markup ... */
    caps = gst_caps_new_simple ("text/x-raw", "format", G_TYPE_STRING,
        "pango-markup", NULL);
    context->postprocess_frame = gst_matroska_demux_check_subtitle_buffer;
    subtitlecontext->check_markup = TRUE;
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_SUBTITLE_SSA)) {
    caps = gst_caps_new_empty_simple ("application/x-ssa");
    context->postprocess_frame = gst_matroska_demux_check_subtitle_buffer;
    subtitlecontext->check_markup = FALSE;
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_SUBTITLE_ASS)) {
    caps = gst_caps_new_empty_simple ("application/x-ass");
    context->postprocess_frame = gst_matroska_demux_check_subtitle_buffer;
    subtitlecontext->check_markup = FALSE;
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_SUBTITLE_USF)) {
    caps = gst_caps_new_empty_simple ("application/x-usf");
    context->postprocess_frame = gst_matroska_demux_check_subtitle_buffer;
    subtitlecontext->check_markup = FALSE;
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_SUBTITLE_VOBSUB)) {
    caps = gst_caps_new_empty_simple ("subpicture/x-dvd");
    ((GstMatroskaTrackContext *) subtitlecontext)->send_dvd_event = TRUE;
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_SUBTITLE_HDMVPGS)) {
    caps = gst_caps_new_empty_simple ("subpicture/x-pgs");
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_SUBTITLE_KATE)) {
    caps = gst_caps_new_empty_simple ("subtitle/x-kate");
    context->stream_headers =
        gst_matroska_parse_xiph_stream_headers (context->codec_priv,
        context->codec_priv_size);
    /* FIXME: mark stream as broken and skip if there are no stream headers */
    context->send_stream_headers = TRUE;
  } else {
    GST_DEBUG ("Unknown subtitle stream: codec_id='%s'", codec_id);
    caps = gst_caps_new_empty_simple ("application/x-subtitle-unknown");
  }

  if (data != NULL && size > 0) {
    GstBuffer *buf;

    buf = gst_buffer_new_wrapped (g_memdup (data, size), size);
    gst_caps_set_simple (caps, "codec_data", GST_TYPE_BUFFER, buf, NULL);
    gst_buffer_unref (buf);
  }

  return caps;
}