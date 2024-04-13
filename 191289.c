qtdemux_tag_add_uint32 (GstQTDemux * qtdemux, GstTagList * taglist,
    const char *tag1, const char *dummy, GNode * node)
{
  GNode *data;
  int len;
  int type;
  guint32 num;

  data = qtdemux_tree_get_child_by_type (node, FOURCC_data);
  if (data) {
    len = QT_UINT32 (data->data);
    type = QT_UINT32 ((guint8 *) data->data + 8);
    GST_DEBUG_OBJECT (qtdemux, "have %s tag, type=%d,len=%d", tag1, type, len);
    /* some files wrongly have a type 0x0f=15, but it should be 0x15 */
    if ((type == 0x00000015 || type == 0x0000000f) && len >= 20) {
      num = QT_UINT32 ((guint8 *) data->data + 16);
      if (num) {
        /* do not add num=0 */
        GST_DEBUG_OBJECT (qtdemux, "adding tag %d", num);
        gst_tag_list_add (taglist, GST_TAG_MERGE_REPLACE, tag1, num, NULL);
      }
    }
  }
}