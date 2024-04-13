qtdemux_generic_caps (GstQTDemux * qtdemux, QtDemuxStream * stream,
    guint32 fourcc, const guint8 * stsd_data, gchar ** codec_name)
{
  GstCaps *caps;

  switch (fourcc) {
    case FOURCC_m1v:
      _codec ("MPEG 1 video");
      caps = gst_caps_new_simple ("video/mpeg", "mpegversion", G_TYPE_INT, 1,
          "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
      break;
    default:
      caps = NULL;
      break;
  }
  return caps;
}