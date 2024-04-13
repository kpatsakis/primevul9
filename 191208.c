gst_qtdemux_class_init (GstQTDemuxClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class->dispose = gst_qtdemux_dispose;

  gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_qtdemux_change_state);
#if 0
  gstelement_class->set_index = GST_DEBUG_FUNCPTR (gst_qtdemux_set_index);
  gstelement_class->get_index = GST_DEBUG_FUNCPTR (gst_qtdemux_get_index);
#endif

  gst_tag_register_musicbrainz_tags ();

  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_qtdemux_sink_template);
  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_qtdemux_videosrc_template);
  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_qtdemux_audiosrc_template);
  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_qtdemux_subsrc_template);
  gst_element_class_set_static_metadata (gstelement_class, "QuickTime demuxer",
      "Codec/Demuxer",
      "Demultiplex a QuickTime file into audio and video streams",
      "David Schleef <ds@schleef.org>, Wim Taymans <wim@fluendo.com>");

  GST_DEBUG_CATEGORY_INIT (qtdemux_debug, "qtdemux", 0, "qtdemux plugin");

}