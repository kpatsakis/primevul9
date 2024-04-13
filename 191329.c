qtdemux_tag_add_keywords (GstQTDemux * qtdemux, GstTagList * taglist,
    const char *tag, const char *dummy, GNode * node)
{
  const gchar *env_vars[] = { "GST_QT_TAG_ENCODING", "GST_TAG_ENCODING", NULL };
  guint8 *data;
  char *s, *t, *k = NULL;
  int len;
  int offset;
  int count;

  /* first try normal string tag if major brand not 3GP */
  if (!qtdemux_is_brand_3gp (qtdemux, TRUE)) {
    if (!qtdemux_tag_add_str_full (qtdemux, taglist, tag, dummy, node)) {
      /* hm, that did not work, maybe 3gpp storage in non-3gpp major brand;
       * let's try it 3gpp way after minor safety check */
      data = node->data;
      if (QT_UINT32 (data) < 15 || !qtdemux_is_brand_3gp (qtdemux, FALSE))
        return;
    } else
      return;
  }

  GST_DEBUG_OBJECT (qtdemux, "found 3gpp keyword tag");

  data = node->data;

  len = QT_UINT32 (data);
  if (len < 15)
    goto short_read;

  count = QT_UINT8 (data + 14);
  offset = 15;
  for (; count; count--) {
    gint slen;

    if (offset + 1 > len)
      goto short_read;
    slen = QT_UINT8 (data + offset);
    offset += 1;
    if (offset + slen > len)
      goto short_read;
    s = gst_tag_freeform_string_to_utf8 ((char *) node->data + offset,
        slen, env_vars);
    if (s) {
      GST_DEBUG_OBJECT (qtdemux, "adding keyword %s", GST_STR_NULL (s));
      if (k) {
        t = g_strjoin (",", k, s, NULL);
        g_free (s);
        g_free (k);
        k = t;
      } else {
        k = s;
      }
    } else {
      GST_DEBUG_OBJECT (qtdemux, "failed to convert keyword to UTF-8");
    }
    offset += slen;
  }

done:
  if (k) {
    GST_DEBUG_OBJECT (qtdemux, "adding tag %s", GST_STR_NULL (k));
    gst_tag_list_add (taglist, GST_TAG_MERGE_REPLACE, tag, k, NULL);
  }
  g_free (k);

  return;

  /* ERRORS */
short_read:
  {
    GST_DEBUG_OBJECT (qtdemux, "short read parsing 3GP keywords");
    goto done;
  }
}