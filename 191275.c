gst_qtdemux_get_index (GstElement * element)
{
  GstIndex *result = NULL;
  GstQTDemux *demux = GST_QTDEMUX (element);

  GST_OBJECT_LOCK (demux);
  if (demux->element_index)
    result = gst_object_ref (demux->element_index);
  GST_OBJECT_UNLOCK (demux);

  GST_DEBUG_OBJECT (demux, "Returning index %" GST_PTR_FORMAT, result);

  return result;
}