qtdemux_tag_add_year (GstQTDemux * qtdemux, GstTagList * taglist,
    const char *tag, const char *dummy, GNode * node)
{
  guint16 y;
  GDate *date;
  gint len;

  len = QT_UINT32 (node->data);
  if (len < 14)
    return;

  y = QT_UINT16 ((guint8 *) node->data + 12);
  if (y == 0) {
    GST_DEBUG_OBJECT (qtdemux, "year: %u is not a valid year", y);
    return;
  }
  GST_DEBUG_OBJECT (qtdemux, "year: %u", y);

  date = g_date_new_dmy (1, 1, y);
  gst_tag_list_add (taglist, GST_TAG_MERGE_REPLACE, tag, date, NULL);
  g_date_free (date);
}