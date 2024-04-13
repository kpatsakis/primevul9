gst_matroska_demux_push_dvd_clut_change_event (GstMatroskaDemux * demux,
    GstMatroskaTrackContext * stream)
{
  gchar *buf, *start;

  g_assert (!strcmp (stream->codec_id, GST_MATROSKA_CODEC_ID_SUBTITLE_VOBSUB));

  if (!stream->codec_priv)
    return;

  /* ideally, VobSub private data should be parsed and stored more convenient
   * elsewhere, but for now, only interested in a small part */

  /* make sure we have terminating 0 */
  buf = g_strndup (stream->codec_priv, stream->codec_priv_size);

  /* just locate and parse palette part */
  start = strstr (buf, "palette:");
  if (start) {
    gint i;
    guint32 clut[16];
    guint32 col;
    guint8 r, g, b, y, u, v;

    start += 8;
    while (g_ascii_isspace (*start))
      start++;
    for (i = 0; i < 16; i++) {
      if (sscanf (start, "%06x", &col) != 1)
        break;
      start += 6;
      while ((*start == ',') || g_ascii_isspace (*start))
        start++;
      /* sigh, need to convert this from vobsub pseudo-RGB to YUV */
      r = (col >> 16) & 0xff;
      g = (col >> 8) & 0xff;
      b = col & 0xff;
      y = CLAMP ((0.1494 * r + 0.6061 * g + 0.2445 * b) * 219 / 255 + 16, 0,
          255);
      u = CLAMP (0.6066 * r - 0.4322 * g - 0.1744 * b + 128, 0, 255);
      v = CLAMP (-0.08435 * r - 0.3422 * g + 0.4266 * b + 128, 0, 255);
      clut[i] = (y << 16) | (u << 8) | v;
    }

    /* got them all without problems; build and send event */
    if (i == 16) {
      GstStructure *s;

      s = gst_structure_new ("application/x-gst-dvd", "event", G_TYPE_STRING,
          "dvd-spu-clut-change", "clut00", G_TYPE_INT, clut[0], "clut01",
          G_TYPE_INT, clut[1], "clut02", G_TYPE_INT, clut[2], "clut03",
          G_TYPE_INT, clut[3], "clut04", G_TYPE_INT, clut[4], "clut05",
          G_TYPE_INT, clut[5], "clut06", G_TYPE_INT, clut[6], "clut07",
          G_TYPE_INT, clut[7], "clut08", G_TYPE_INT, clut[8], "clut09",
          G_TYPE_INT, clut[9], "clut10", G_TYPE_INT, clut[10], "clut11",
          G_TYPE_INT, clut[11], "clut12", G_TYPE_INT, clut[12], "clut13",
          G_TYPE_INT, clut[13], "clut14", G_TYPE_INT, clut[14], "clut15",
          G_TYPE_INT, clut[15], NULL);

      gst_pad_push_event (stream->pad,
          gst_event_new_custom (GST_EVENT_CUSTOM_DOWNSTREAM_STICKY, s));
    }
  }
  g_free (buf);
}