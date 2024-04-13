gst_qtdemux_push_pending_newsegment (GstQTDemux * qtdemux)
{
  if (qtdemux->pending_newsegment) {
    gst_qtdemux_push_event (qtdemux, qtdemux->pending_newsegment);
    qtdemux->pending_newsegment = NULL;
  }
}