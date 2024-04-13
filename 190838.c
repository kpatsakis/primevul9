gst_aac_parse_src_event (GstBaseParse * parse, GstEvent * event)
{
  GstAacParse *aacparse = GST_AAC_PARSE (parse);

  if (GST_EVENT_TYPE (event) == GST_EVENT_FLUSH_STOP) {
    aacparse->last_parsed_channels = 0;
    aacparse->last_parsed_sample_rate = 0;
  }

  return GST_BASE_PARSE_CLASS (parent_class)->src_event (parse, event);
}