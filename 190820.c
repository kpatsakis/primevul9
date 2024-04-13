gst_aac_parse_start (GstBaseParse * parse)
{
  GstAacParse *aacparse;

  aacparse = GST_AAC_PARSE (parse);
  GST_DEBUG ("start");
  aacparse->frame_samples = 1024;
  gst_base_parse_set_min_frame_size (GST_BASE_PARSE (aacparse), ADTS_MAX_SIZE);
  aacparse->sent_codec_tag = FALSE;
  aacparse->last_parsed_channels = 0;
  aacparse->last_parsed_sample_rate = 0;
  return TRUE;
}