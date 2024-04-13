gst_matroska_demux_handle_src_query (GstPad * pad, GstObject * parent,
    GstQuery * query)
{
  GstMatroskaDemux *demux = GST_MATROSKA_DEMUX (parent);

  return gst_matroska_demux_query (demux, pad, query);
}