gst_matroska_demux_check_subtitle_buffer (GstElement * element,
    GstMatroskaTrackContext * stream, GstBuffer ** buf)
{
  GstMatroskaTrackSubtitleContext *sub_stream;
  const gchar *encoding;
  GError *err = NULL;
  GstBuffer *newbuf;
  gchar *utf8;
  GstMapInfo map;
  gboolean needs_unmap = TRUE;

  sub_stream = (GstMatroskaTrackSubtitleContext *) stream;

  if (!gst_buffer_get_size (*buf) || !gst_buffer_map (*buf, &map, GST_MAP_READ))
    return GST_FLOW_OK;

  /* The subtitle buffer we push out should not include a NUL terminator as
   * part of the data. */
  if (map.data[map.size - 1] == '\0') {
    gst_buffer_set_size (*buf, map.size - 1);
    gst_buffer_unmap (*buf, &map);
    gst_buffer_map (*buf, &map, GST_MAP_READ);
  }

  if (!sub_stream->invalid_utf8) {
    if (g_utf8_validate ((gchar *) map.data, map.size, NULL)) {
      goto next;
    }
    GST_WARNING_OBJECT (element, "subtitle stream %" G_GUINT64_FORMAT
        " is not valid UTF-8, this is broken according to the matroska"
        " specification", stream->num);
    sub_stream->invalid_utf8 = TRUE;
  }

  /* file with broken non-UTF8 subtitle, do the best we can do to fix it */
  encoding = g_getenv ("GST_SUBTITLE_ENCODING");
  if (encoding == NULL || *encoding == '\0') {
    /* if local encoding is UTF-8 and no encoding specified
     * via the environment variable, assume ISO-8859-15 */
    if (g_get_charset (&encoding)) {
      encoding = "ISO-8859-15";
    }
  }

  utf8 =
      g_convert_with_fallback ((gchar *) map.data, map.size, "UTF-8", encoding,
      (char *) "*", NULL, NULL, &err);

  if (err) {
    GST_LOG_OBJECT (element, "could not convert string from '%s' to UTF-8: %s",
        encoding, err->message);
    g_error_free (err);
    g_free (utf8);

    /* invalid input encoding, fall back to ISO-8859-15 (always succeeds) */
    encoding = "ISO-8859-15";
    utf8 =
        g_convert_with_fallback ((gchar *) map.data, map.size, "UTF-8",
        encoding, (char *) "*", NULL, NULL, NULL);
  }

  GST_LOG_OBJECT (element, "converted subtitle text from %s to UTF-8 %s",
      encoding, (err) ? "(using ISO-8859-15 as fallback)" : "");

  if (utf8 == NULL)
    utf8 = g_strdup ("invalid subtitle");

  newbuf = gst_buffer_new_wrapped (utf8, strlen (utf8));
  gst_buffer_unmap (*buf, &map);
  gst_buffer_copy_into (newbuf, *buf,
      GST_BUFFER_COPY_TIMESTAMPS | GST_BUFFER_COPY_FLAGS | GST_BUFFER_COPY_META,
      0, -1);
  gst_buffer_unref (*buf);

  *buf = newbuf;
  gst_buffer_map (*buf, &map, GST_MAP_READ);

next:

  if (sub_stream->check_markup) {
    /* caps claim markup text, so we need to escape text,
     * except if text is already markup and then needs no further escaping */
    sub_stream->seen_markup_tag = sub_stream->seen_markup_tag ||
        gst_matroska_demux_subtitle_chunk_has_tag (element, (gchar *) map.data);

    if (!sub_stream->seen_markup_tag) {
      utf8 = g_markup_escape_text ((gchar *) map.data, map.size);

      newbuf = gst_buffer_new_wrapped (utf8, strlen (utf8));
      gst_buffer_unmap (*buf, &map);
      gst_buffer_copy_into (newbuf, *buf,
          GST_BUFFER_COPY_TIMESTAMPS | GST_BUFFER_COPY_FLAGS |
          GST_BUFFER_COPY_META, 0, -1);
      gst_buffer_unref (*buf);

      *buf = newbuf;
      needs_unmap = FALSE;
    }
  }

  if (needs_unmap)
    gst_buffer_unmap (*buf, &map);

  return GST_FLOW_OK;
}