remove_fields (GstCaps * caps)
{
  guint i, n;

  n = gst_caps_get_size (caps);
  for (i = 0; i < n; i++) {
    GstStructure *s = gst_caps_get_structure (caps, i);

    gst_structure_remove_field (s, "framed");
  }
}