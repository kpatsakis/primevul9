gst_qtdemux_process_buffer (GstQTDemux * qtdemux, QtDemuxStream * stream,
    GstBuffer * buf)
{
  GstMapInfo map;
  guint nsize = 0;
  gchar *str;

  /* not many cases for now */
  if (G_UNLIKELY (stream->fourcc == FOURCC_mp4s)) {
    /* send a one time dvd clut event */
    if (stream->pending_event && stream->pad)
      gst_pad_push_event (stream->pad, stream->pending_event);
    stream->pending_event = NULL;
  }

  if (G_UNLIKELY (stream->subtype != FOURCC_text
          && stream->subtype != FOURCC_sbtl &&
          stream->subtype != FOURCC_subp)) {
    return buf;
  }

  gst_buffer_map (buf, &map, GST_MAP_READ);

  /* empty buffer is sent to terminate previous subtitle */
  if (map.size <= 2) {
    gst_buffer_unmap (buf, &map);
    gst_buffer_unref (buf);
    return NULL;
  }
  if (stream->subtype == FOURCC_subp) {
    /* That's all the processing needed for subpictures */
    gst_buffer_unmap (buf, &map);
    return buf;
  }

  nsize = GST_READ_UINT16_BE (map.data);
  nsize = MIN (nsize, map.size - 2);

  GST_LOG_OBJECT (qtdemux, "3GPP timed text subtitle: %d/%" G_GSIZE_FORMAT "",
      nsize, map.size);

  /* takes care of UTF-8 validation or UTF-16 recognition,
   * no other encoding expected */
  str = gst_tag_freeform_string_to_utf8 ((gchar *) map.data + 2, nsize, NULL);
  gst_buffer_unmap (buf, &map);
  if (str) {
    gst_buffer_unref (buf);
    buf = _gst_buffer_new_wrapped (str, strlen (str), g_free);
  } else {
    /* this should not really happen unless the subtitle is corrupted */
    gst_buffer_unref (buf);
    buf = NULL;
  }

  /* FIXME ? convert optional subsequent style info to markup */

  return buf;
}