gst_aac_parse_class_init (GstAacParseClass * klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);
  GstBaseParseClass *parse_class = GST_BASE_PARSE_CLASS (klass);

  GST_DEBUG_CATEGORY_INIT (aacparse_debug, "aacparse", 0,
      "AAC audio stream parser");

  gst_element_class_add_static_pad_template (element_class, &sink_template);
  gst_element_class_add_static_pad_template (element_class, &src_template);

  gst_element_class_set_static_metadata (element_class,
      "AAC audio stream parser", "Codec/Parser/Audio",
      "Advanced Audio Coding parser", "Stefan Kost <stefan.kost@nokia.com>");

  parse_class->start = GST_DEBUG_FUNCPTR (gst_aac_parse_start);
  parse_class->stop = GST_DEBUG_FUNCPTR (gst_aac_parse_stop);
  parse_class->set_sink_caps = GST_DEBUG_FUNCPTR (gst_aac_parse_sink_setcaps);
  parse_class->get_sink_caps = GST_DEBUG_FUNCPTR (gst_aac_parse_sink_getcaps);
  parse_class->handle_frame = GST_DEBUG_FUNCPTR (gst_aac_parse_handle_frame);
  parse_class->pre_push_frame =
      GST_DEBUG_FUNCPTR (gst_aac_parse_pre_push_frame);
  parse_class->src_event = GST_DEBUG_FUNCPTR (gst_aac_parse_src_event);
}