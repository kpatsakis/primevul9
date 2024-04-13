qtdemux_parse_udta (GstQTDemux * qtdemux, GstTagList * taglist, GNode * udta)
{
  GNode *meta;
  GNode *ilst;
  GNode *xmp_;
  GNode *node;
  gint i;
  GstQtDemuxTagList demuxtaglist;

  demuxtaglist.demux = qtdemux;
  demuxtaglist.taglist = taglist;

  meta = qtdemux_tree_get_child_by_type (udta, FOURCC_meta);
  if (meta != NULL) {
    ilst = qtdemux_tree_get_child_by_type (meta, FOURCC_ilst);
    if (ilst == NULL) {
      GST_LOG_OBJECT (qtdemux, "no ilst");
      return;
    }
  } else {
    ilst = udta;
    GST_LOG_OBJECT (qtdemux, "no meta so using udta itself");
  }

  i = 0;
  while (i < G_N_ELEMENTS (add_funcs)) {
    node = qtdemux_tree_get_child_by_type (ilst, add_funcs[i].fourcc);
    if (node) {
      gint len;

      len = QT_UINT32 (node->data);
      if (len < 12) {
        GST_DEBUG_OBJECT (qtdemux, "too small tag atom %" GST_FOURCC_FORMAT,
            GST_FOURCC_ARGS (add_funcs[i].fourcc));
      } else {
        add_funcs[i].func (qtdemux, taglist, add_funcs[i].gst_tag,
            add_funcs[i].gst_tag_bis, node);
      }
      g_node_destroy (node);
    } else {
      i++;
    }
  }

  /* parsed nodes have been removed, pass along remainder as blob */
  g_node_children_foreach (ilst, G_TRAVERSE_ALL,
      (GNodeForeachFunc) qtdemux_tag_add_blob, &demuxtaglist);

  /* parse up XMP_ node if existing */
  xmp_ = qtdemux_tree_get_child_by_type (udta, FOURCC_XMP_);
  if (xmp_ != NULL) {
    GstBuffer *buf;
    GstTagList *xmptaglist;

    buf = _gst_buffer_new_wrapped (((guint8 *) xmp_->data) + 8,
        QT_UINT32 ((guint8 *) xmp_->data) - 8, NULL);
    xmptaglist = gst_tag_list_from_xmp_buffer (buf);
    gst_buffer_unref (buf);

    qtdemux_handle_xmp_taglist (qtdemux, taglist, xmptaglist);
  } else {
    GST_DEBUG_OBJECT (qtdemux, "No XMP_ node found");
  }
}