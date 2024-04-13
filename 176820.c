gst_matroska_demux_set_index (GstElement * element, GstIndex * index)
{
  GstMatroskaDemux *demux = GST_MATROSKA_DEMUX (element);

  GST_OBJECT_LOCK (demux);
  if (demux->common.element_index)
    gst_object_unref (demux->common.element_index);
  demux->common.element_index = index ? gst_object_ref (index) : NULL;
  GST_OBJECT_UNLOCK (demux);
  GST_DEBUG_OBJECT (demux, "Set index %" GST_PTR_FORMAT,
      demux->common.element_index);
}