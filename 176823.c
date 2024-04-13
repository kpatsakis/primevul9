gst_matroska_demux_finalize (GObject * object)
{
  GstMatroskaDemux *demux = GST_MATROSKA_DEMUX (object);

  gst_matroska_read_common_finalize (&demux->common);
  gst_flow_combiner_free (demux->flowcombiner);
  G_OBJECT_CLASS (parent_class)->finalize (object);
}