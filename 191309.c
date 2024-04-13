qtdemux_add_double_tag_from_str (GstQTDemux * demux, GstTagList * taglist,
    const gchar * tag, guint8 * data, guint32 datasize)
{
  gdouble value;
  gchar *datacopy;

  /* make a copy to have \0 at the end */
  datacopy = g_strndup ((gchar *) data, datasize);

  /* convert the str to double */
  if (sscanf (datacopy, "%lf", &value) == 1) {
    GST_DEBUG_OBJECT (demux, "adding tag: %s [%s]", tag, datacopy);
    gst_tag_list_add (taglist, GST_TAG_MERGE_REPLACE, tag, value, NULL);
  } else {
    GST_WARNING_OBJECT (demux, "Failed to parse double from string: %s",
        datacopy);
  }
  g_free (datacopy);
}