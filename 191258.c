gst_qtdemux_setcaps (GstQTDemux * demux, GstCaps * caps)
{
  GstStructure *structure;
  const gchar *variant;
  const GstCaps *mediacaps = NULL;

  GST_DEBUG_OBJECT (demux, "Sink set caps: %" GST_PTR_FORMAT, caps);

  structure = gst_caps_get_structure (caps, 0);
  variant = gst_structure_get_string (structure, "variant");

  if (variant && strcmp (variant, "mss-fragmented") == 0) {
    QtDemuxStream *stream;
    const GValue *value;

    demux->fragmented = TRUE;
    demux->mss_mode = TRUE;

    if (demux->n_streams > 1) {
      /* can't do this, we can only renegotiate for another mss format */
      return FALSE;
    }

    value = gst_structure_get_value (structure, "media-caps");
    /* create stream */
    if (value) {
      const GValue *timescale_v;

      /* TODO update when stream changes during playback */

      if (demux->n_streams == 0) {
        stream = _create_stream ();
        demux->streams[demux->n_streams] = stream;
        demux->n_streams = 1;
      } else {
        stream = demux->streams[0];
      }

      timescale_v = gst_structure_get_value (structure, "timescale");
      if (timescale_v) {
        stream->timescale = g_value_get_uint64 (timescale_v);
      } else {
        /* default mss timescale */
        stream->timescale = 10000000;
      }
      demux->timescale = stream->timescale;

      mediacaps = gst_value_get_caps (value);
      if (!stream->caps || !gst_caps_is_equal_fixed (mediacaps, stream->caps)) {
        GST_DEBUG_OBJECT (demux, "We have a new caps %" GST_PTR_FORMAT,
            mediacaps);
        stream->new_caps = TRUE;
      }
      gst_caps_replace (&stream->caps, (GstCaps *) mediacaps);
      structure = gst_caps_get_structure (mediacaps, 0);
      if (g_str_has_prefix (gst_structure_get_name (structure), "video")) {
        stream->subtype = FOURCC_vide;

        gst_structure_get_int (structure, "width", &stream->width);
        gst_structure_get_int (structure, "height", &stream->height);
        gst_structure_get_fraction (structure, "framerate", &stream->fps_n,
            &stream->fps_d);
      } else if (g_str_has_prefix (gst_structure_get_name (structure), "audio")) {
        gint rate = 0;
        stream->subtype = FOURCC_soun;
        gst_structure_get_int (structure, "channels", &stream->n_channels);
        gst_structure_get_int (structure, "rate", &rate);
        stream->rate = rate;
      }
    }
    gst_caps_replace (&demux->media_caps, (GstCaps *) mediacaps);
  } else {
    demux->mss_mode = FALSE;
  }

  return TRUE;
}