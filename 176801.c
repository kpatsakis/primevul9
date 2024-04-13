gst_matroska_demux_add_stream_headers_to_caps (GstMatroskaDemux * demux,
    GstBufferList * list, GstCaps * caps)
{
  GstStructure *s;
  GValue arr_val = G_VALUE_INIT;
  GValue buf_val = G_VALUE_INIT;
  gint i, num;

  g_assert (gst_caps_is_writable (caps));

  g_value_init (&arr_val, GST_TYPE_ARRAY);
  g_value_init (&buf_val, GST_TYPE_BUFFER);

  num = gst_buffer_list_length (list);
  for (i = 0; i < num; ++i) {
    g_value_set_boxed (&buf_val, gst_buffer_list_get (list, i));
    gst_value_array_append_value (&arr_val, &buf_val);
  }

  s = gst_caps_get_structure (caps, 0);
  gst_structure_take_value (s, "streamheader", &arr_val);
  g_value_unset (&buf_val);
}