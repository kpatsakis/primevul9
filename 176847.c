gst_matroska_demux_subtitle_chunk_has_tag (GstElement * element,
    const gchar * text)
{
  gchar *tag;

  g_return_val_if_fail (text != NULL, FALSE);

  /* yes, this might all lead to false positives ... */
  tag = (gchar *) text;
  while ((tag = strchr (tag, '<'))) {
    tag++;
    if (*tag != '\0' && *(tag + 1) == '>') {
      /* some common convenience ones */
      /* maybe any character will do here ? */
      switch (*tag) {
        case 'b':
        case 'i':
        case 'u':
        case 's':
          return TRUE;
        default:
          return FALSE;
      }
    }
  }

  if (strstr (text, "<span"))
    return TRUE;

  return FALSE;
}