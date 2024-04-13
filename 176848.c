gst_matroska_demux_element_query (GstElement * element, GstQuery * query)
{
  return gst_matroska_demux_query (GST_MATROSKA_DEMUX (element), NULL, query);
}