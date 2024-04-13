gst_qtdemux_dispose (GObject * object)
{
  GstQTDemux *qtdemux = GST_QTDEMUX (object);

  if (qtdemux->adapter) {
    g_object_unref (G_OBJECT (qtdemux->adapter));
    qtdemux->adapter = NULL;
  }
  gst_flow_combiner_free (qtdemux->flowcombiner);
  g_queue_foreach (&qtdemux->protection_event_queue, (GFunc) gst_event_unref,
      NULL);
  g_queue_clear (&qtdemux->protection_event_queue);

  g_free (qtdemux->cenc_aux_info_sizes);
  qtdemux->cenc_aux_info_sizes = NULL;

  G_OBJECT_CLASS (parent_class)->dispose (object);
}