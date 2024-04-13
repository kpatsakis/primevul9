qtdemux_tree_get_sibling_by_type_full (GNode * node, guint32 fourcc,
    GstByteReader * parser)
{
  GNode *child;
  guint8 *buffer;
  guint32 child_fourcc, child_len;

  for (child = g_node_next_sibling (node); child;
      child = g_node_next_sibling (child)) {
    buffer = (guint8 *) child->data;

    child_fourcc = QT_FOURCC (buffer + 4);

    if (child_fourcc == fourcc) {
      if (parser) {
        child_len = QT_UINT32 (buffer);
        if (G_UNLIKELY (child_len < (4 + 4)))
          return NULL;
        /* FIXME: must verify if atom length < parent atom length */
        gst_byte_reader_init (parser, buffer + (4 + 4), child_len - (4 + 4));
      }
      return child;
    }
  }
  return NULL;
}