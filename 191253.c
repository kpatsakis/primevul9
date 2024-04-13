qtdemux_tag_add_covr (GstQTDemux * qtdemux, GstTagList * taglist,
    const char *tag1, const char *dummy, GNode * node)
{
  GNode *data;
  int len;
  int type;
  GstSample *sample;

  data = qtdemux_tree_get_child_by_type (node, FOURCC_data);
  if (data) {
    len = QT_UINT32 (data->data);
    type = QT_UINT32 ((guint8 *) data->data + 8);
    GST_DEBUG_OBJECT (qtdemux, "have covr tag, type=%d,len=%d", type, len);
    if ((type == 0x0000000d || type == 0x0000000e) && len > 16) {
      if ((sample =
              gst_tag_image_data_to_image_sample ((guint8 *) data->data + 16,
                  len - 16, GST_TAG_IMAGE_TYPE_NONE))) {
        GST_DEBUG_OBJECT (qtdemux, "adding tag size %d", len - 16);
        gst_tag_list_add (taglist, GST_TAG_MERGE_REPLACE, tag1, sample, NULL);
        gst_sample_unref (sample);
      }
    }
  }
}