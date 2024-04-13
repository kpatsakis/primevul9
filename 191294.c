gst_qtdemux_set_index (GstElement * element, GstIndex * index)
{
  GstQTDemux *demux = GST_QTDEMUX (element);

  GST_OBJECT_LOCK (demux);
  if (demux->element_index)
    gst_object_unref (demux->element_index);
  if (index) {
    demux->element_index = gst_object_ref (index);
  } else {
    demux->element_index = NULL;
  }
  GST_OBJECT_UNLOCK (demux);
  /* object lock might be taken again */
  if (index)
    gst_index_get_writer_id (index, GST_OBJECT (element), &demux->index_id);
  GST_DEBUG_OBJECT (demux, "Set index %" GST_PTR_FORMAT "for writer id %d",
      demux->element_index, demux->index_id);
}