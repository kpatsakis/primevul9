qtdemux_process_redirects (GstQTDemux * qtdemux, GList * references)
{
  GstQtReference *best;
  GstStructure *s;
  GstMessage *msg;
  GValue list_val = { 0, };
  GList *l;

  g_assert (references != NULL);

  references = g_list_sort (references, qtdemux_redirects_sort_func);

  best = (GstQtReference *) references->data;

  g_value_init (&list_val, GST_TYPE_LIST);

  for (l = references; l != NULL; l = l->next) {
    GstQtReference *ref = (GstQtReference *) l->data;
    GValue struct_val = { 0, };

    ref->structure = gst_structure_new ("redirect",
        "new-location", G_TYPE_STRING, ref->location, NULL);

    if (ref->min_req_bitrate > 0) {
      gst_structure_set (ref->structure, "minimum-bitrate", G_TYPE_INT,
          ref->min_req_bitrate, NULL);
    }

    g_value_init (&struct_val, GST_TYPE_STRUCTURE);
    g_value_set_boxed (&struct_val, ref->structure);
    gst_value_list_append_value (&list_val, &struct_val);
    g_value_unset (&struct_val);
    /* don't free anything here yet, since we need best->structure below */
  }

  g_assert (best != NULL);
  s = gst_structure_copy (best->structure);

  if (g_list_length (references) > 1) {
    gst_structure_set_value (s, "locations", &list_val);
  }

  g_value_unset (&list_val);

  for (l = references; l != NULL; l = l->next) {
    GstQtReference *ref = (GstQtReference *) l->data;

    gst_structure_free (ref->structure);
    g_free (ref->location);
    g_free (ref);
  }
  g_list_free (references);

  GST_INFO_OBJECT (qtdemux, "posting redirect message: %" GST_PTR_FORMAT, s);
  msg = gst_message_new_element (GST_OBJECT_CAST (qtdemux), s);
  gst_element_post_message (GST_ELEMENT_CAST (qtdemux), msg);
  qtdemux->posted_redirect = TRUE;
}