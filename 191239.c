qtdemux_parse_pssh (GstQTDemux * qtdemux, GNode * node)
{
  gchar *sysid_string;
  guint32 pssh_size = QT_UINT32 (node->data);
  GstBuffer *pssh = NULL;
  GstEvent *event = NULL;
  guint32 parent_box_type;
  gint i;

  if (G_UNLIKELY (pssh_size < 32U)) {
    GST_ERROR_OBJECT (qtdemux, "invalid box size");
    return FALSE;
  }

  sysid_string =
      qtdemux_uuid_bytes_to_string ((const guint8 *) node->data + 12);

  gst_qtdemux_append_protection_system_id (qtdemux, sysid_string);

  pssh = gst_buffer_new_wrapped (g_memdup (node->data, pssh_size), pssh_size);
  GST_LOG_OBJECT (qtdemux, "cenc pssh size: %" G_GSIZE_FORMAT,
      gst_buffer_get_size (pssh));

  parent_box_type = QT_FOURCC ((const guint8 *) node->parent->data + 4);

  /* Push an event containing the pssh box onto the queues of all streams. */
  event = gst_event_new_protection (sysid_string, pssh,
      (parent_box_type == FOURCC_moov) ? "isobmff/moov" : "isobmff/moof");
  for (i = 0; i < qtdemux->n_streams; ++i) {
    g_queue_push_tail (&qtdemux->streams[i]->protection_scheme_event_queue,
        gst_event_ref (event));
  }
  g_free (sysid_string);
  gst_event_unref (event);
  gst_buffer_unref (pssh);
  return TRUE;
}