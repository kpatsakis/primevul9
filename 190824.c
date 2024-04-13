gst_aac_parse_init (GstAacParse * aacparse)
{
  GST_DEBUG ("initialized");
  GST_PAD_SET_ACCEPT_INTERSECT (GST_BASE_PARSE_SINK_PAD (aacparse));
  GST_PAD_SET_ACCEPT_TEMPLATE (GST_BASE_PARSE_SINK_PAD (aacparse));

  aacparse->last_parsed_sample_rate = 0;
  aacparse->last_parsed_channels = 0;
}