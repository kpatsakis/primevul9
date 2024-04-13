qtdemux_tag_add_num (GstQTDemux * qtdemux, GstTagList * taglist,
    const char *tag1, const char *tag2, GNode * node)
{
  GNode *data;
  int len;
  int type;
  int n1, n2;

  data = qtdemux_tree_get_child_by_type (node, FOURCC_data);
  if (data) {
    len = QT_UINT32 (data->data);
    type = QT_UINT32 ((guint8 *) data->data + 8);
    if (type == 0x00000000 && len >= 22) {
      n1 = QT_UINT16 ((guint8 *) data->data + 18);
      n2 = QT_UINT16 ((guint8 *) data->data + 20);
      if (n1 > 0) {
        GST_DEBUG_OBJECT (qtdemux, "adding tag %s=%d", tag1, n1);
        gst_tag_list_add (taglist, GST_TAG_MERGE_REPLACE, tag1, n1, NULL);
      }
      if (n2 > 0) {
        GST_DEBUG_OBJECT (qtdemux, "adding tag %s=%d", tag2, n2);
        gst_tag_list_add (taglist, GST_TAG_MERGE_REPLACE, tag2, n2, NULL);
      }
    }
  }
}