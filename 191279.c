qtdemux_parse_sidx (GstQTDemux * qtdemux, const guint8 * buffer, gint length)
{
  GstSidxParser sidx_parser;
  GstIsoffParserResult res;
  guint consumed;

  gst_isoff_qt_sidx_parser_init (&sidx_parser);

  res =
      gst_isoff_qt_sidx_parser_add_data (&sidx_parser, buffer, length,
      &consumed);
  GST_DEBUG_OBJECT (qtdemux, "sidx parse result: %d", res);
  if (res == GST_ISOFF_QT_PARSER_DONE) {
    check_update_duration (qtdemux, sidx_parser.cumulative_pts);
  }
  gst_isoff_qt_sidx_parser_clear (&sidx_parser);
}