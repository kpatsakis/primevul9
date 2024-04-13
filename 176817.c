gst_matroska_demux_class_init (GstMatroskaDemuxClass * klass)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;
  GstElementClass *gstelement_class = (GstElementClass *) klass;

  GST_DEBUG_CATEGORY_INIT (matroskademux_debug, "matroskademux", 0,
      "Matroska demuxer");

  gobject_class->finalize = gst_matroska_demux_finalize;

  gobject_class->get_property = gst_matroska_demux_get_property;
  gobject_class->set_property = gst_matroska_demux_set_property;

  g_object_class_install_property (gobject_class, PROP_MAX_GAP_TIME,
      g_param_spec_uint64 ("max-gap-time", "Maximum gap time",
          "The demuxer sends out segment events for skipping "
          "gaps longer than this (0 = disabled).", 0, G_MAXUINT64,
          DEFAULT_MAX_GAP_TIME, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_MAX_BACKTRACK_DISTANCE,
      g_param_spec_uint ("max-backtrack-distance",
          "Maximum backtrack distance",
          "Maximum backtrack distance in seconds when seeking without "
          "and index in pull mode and search for a keyframe "
          "(0 = disable backtracking).",
          0, G_MAXUINT, DEFAULT_MAX_BACKTRACK_DISTANCE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gstelement_class->change_state =
      GST_DEBUG_FUNCPTR (gst_matroska_demux_change_state);
  gstelement_class->send_event =
      GST_DEBUG_FUNCPTR (gst_matroska_demux_element_send_event);
  gstelement_class->query =
      GST_DEBUG_FUNCPTR (gst_matroska_demux_element_query);
#if 0
  gstelement_class->set_index =
      GST_DEBUG_FUNCPTR (gst_matroska_demux_set_index);
  gstelement_class->get_index =
      GST_DEBUG_FUNCPTR (gst_matroska_demux_get_index);
#endif

  gst_element_class_add_static_pad_template (gstelement_class,
      &video_src_templ);
  gst_element_class_add_static_pad_template (gstelement_class,
      &audio_src_templ);
  gst_element_class_add_static_pad_template (gstelement_class,
      &subtitle_src_templ);
  gst_element_class_add_static_pad_template (gstelement_class, &sink_templ);

  gst_element_class_set_static_metadata (gstelement_class, "Matroska demuxer",
      "Codec/Demuxer",
      "Demuxes Matroska/WebM streams into video/audio/subtitles",
      "GStreamer maintainers <gstreamer-devel@lists.freedesktop.org>");
}