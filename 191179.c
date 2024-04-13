qtdemux_tag_add_id32 (GstQTDemux * demux, GstTagList * taglist, const char *tag,
    const char *tag_bis, GNode * node)
{
  guint8 *data;
  GstBuffer *buf;
  guint len;
  GstTagList *id32_taglist = NULL;

  GST_LOG_OBJECT (demux, "parsing ID32");

  data = node->data;
  len = GST_READ_UINT32_BE (data);

  /* need at least full box and language tag */
  if (len < 12 + 2)
    return;

  buf = gst_buffer_new_allocate (NULL, len - 14, NULL);
  gst_buffer_fill (buf, 0, data + 14, len - 14);

  id32_taglist = gst_tag_list_from_id3v2_tag (buf);
  if (id32_taglist) {
    GST_LOG_OBJECT (demux, "parsing ok");
    gst_tag_list_insert (taglist, id32_taglist, GST_TAG_MERGE_KEEP);
    gst_tag_list_unref (id32_taglist);
  } else {
    GST_LOG_OBJECT (demux, "parsing failed");
  }

  gst_buffer_unref (buf);
}