qtdemux_tag_add_gnre (GstQTDemux * qtdemux, GstTagList * taglist,
    const char *tag, const char *dummy, GNode * node)
{
  GNode *data;

  data = qtdemux_tree_get_child_by_type (node, FOURCC_data);

  /* re-route to normal string tag if major brand says so
   * or no data atom and compatible brand suggests so */
  if (qtdemux_is_brand_3gp (qtdemux, TRUE) ||
      (qtdemux_is_brand_3gp (qtdemux, FALSE) && !data)) {
    qtdemux_tag_add_str (qtdemux, taglist, tag, dummy, node);
    return;
  }

  if (data) {
    guint len, type, n;

    len = QT_UINT32 (data->data);
    type = QT_UINT32 ((guint8 *) data->data + 8);
    if (type == 0x00000000 && len >= 18) {
      n = QT_UINT16 ((guint8 *) data->data + 16);
      if (n > 0) {
        const gchar *genre;

        genre = gst_tag_id3_genre_get (n - 1);
        if (genre != NULL) {
          GST_DEBUG_OBJECT (qtdemux, "adding %d [%s]", n, genre);
          gst_tag_list_add (taglist, GST_TAG_MERGE_REPLACE, tag, genre, NULL);
        }
      }
    }
  }
}