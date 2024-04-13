qtdemux_sub_caps (GstQTDemux * qtdemux, QtDemuxStream * stream,
    guint32 fourcc, const guint8 * stsd_data, gchar ** codec_name)
{
  GstCaps *caps;

  GST_DEBUG_OBJECT (qtdemux, "resolve fourcc 0x%08x", GUINT32_TO_BE (fourcc));

  switch (fourcc) {
    case FOURCC_mp4s:
      _codec ("DVD subtitle");
      caps = gst_caps_new_empty_simple ("subpicture/x-dvd");
      stream->need_process = TRUE;
      break;
    case FOURCC_text:
      _codec ("Quicktime timed text");
      goto text;
    case FOURCC_tx3g:
      _codec ("3GPP timed text");
    text:
      caps = gst_caps_new_simple ("text/x-raw", "format", G_TYPE_STRING,
          "utf8", NULL);
      /* actual text piece needs to be extracted */
      stream->need_process = TRUE;
      break;
    case FOURCC_stpp:
      _codec ("XML subtitles");
      caps = gst_caps_new_empty_simple ("application/ttml+xml");
      break;
    default:
    {
      char *s, fourstr[5];

      g_snprintf (fourstr, 5, "%" GST_FOURCC_FORMAT, GST_FOURCC_ARGS (fourcc));
      s = g_strdup_printf ("text/x-gst-fourcc-%s", g_strstrip (fourstr));
      caps = gst_caps_new_empty_simple (s);
      g_free (s);
      break;
    }
  }
  return caps;
}