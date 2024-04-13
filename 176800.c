gst_matroska_demux_get_index (GstElement * element)
{
  GstIndex *result = NULL;
  GstMatroskaDemux *demux = GST_MATROSKA_DEMUX (element);

  GST_OBJECT_LOCK (demux);
  if (demux->common.element_index)
    result = gst_object_ref (demux->common.element_index);
  GST_OBJECT_UNLOCK (demux);

  GST_DEBUG_OBJECT (demux, "Returning index %" GST_PTR_FORMAT, result);

  return result;
}