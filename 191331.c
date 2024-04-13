qtdemux_tag_add_blob (GNode * node, GstQtDemuxTagList * qtdemuxtaglist)
{
  gint len;
  guint8 *data;
  GstBuffer *buf;
  gchar *media_type;
  const gchar *style;
  GstSample *sample;
  GstStructure *s;
  guint i;
  guint8 ndata[4];
  GstQTDemux *demux = qtdemuxtaglist->demux;
  GstTagList *taglist = qtdemuxtaglist->taglist;

  data = node->data;
  len = QT_UINT32 (data);
  buf = gst_buffer_new_and_alloc (len);
  gst_buffer_fill (buf, 0, data, len);

  /* heuristic to determine style of tag */
  if (QT_FOURCC (data + 4) == FOURCC_____ ||
      (len > 8 + 12 && QT_FOURCC (data + 12) == FOURCC_data))
    style = "itunes";
  else if (demux->major_brand == FOURCC_qt__)
    style = "quicktime";
  /* fall back to assuming iso/3gp tag style */
  else
    style = "iso";

  /* santize the name for the caps. */
  for (i = 0; i < 4; i++) {
    guint8 d = data[4 + i];
    if (g_ascii_isalnum (d))
      ndata[i] = g_ascii_tolower (d);
    else
      ndata[i] = '_';
  }

  media_type = g_strdup_printf ("application/x-gst-qt-%c%c%c%c-tag",
      ndata[0], ndata[1], ndata[2], ndata[3]);
  GST_DEBUG_OBJECT (demux, "media type %s", media_type);

  s = gst_structure_new (media_type, "style", G_TYPE_STRING, style, NULL);
  sample = gst_sample_new (buf, NULL, NULL, s);
  gst_buffer_unref (buf);
  g_free (media_type);

  GST_DEBUG_OBJECT (demux, "adding private tag; size %d, info %" GST_PTR_FORMAT,
      len, s);

  gst_tag_list_add (taglist, GST_TAG_MERGE_APPEND,
      GST_QT_DEMUX_PRIVATE_TAG, sample, NULL);

  gst_sample_unref (sample);
}