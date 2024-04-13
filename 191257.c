qtdemux_tag_add_location (GstQTDemux * qtdemux, GstTagList * taglist,
    const char *tag, const char *dummy, GNode * node)
{
  const gchar *env_vars[] = { "GST_QT_TAG_ENCODING", "GST_TAG_ENCODING", NULL };
  int offset;
  char *name;
  gchar *data;
  gdouble longitude, latitude, altitude;
  gint len;

  len = QT_UINT32 (node->data);
  if (len <= 14)
    goto short_read;

  data = node->data;
  offset = 14;

  /* TODO: language code skipped */

  name = gst_tag_freeform_string_to_utf8 (data + offset, -1, env_vars);

  if (!name) {
    /* do not alarm in trivial case, but bail out otherwise */
    if (*(data + offset) != 0) {
      GST_DEBUG_OBJECT (qtdemux, "failed to convert %s tag to UTF-8, "
          "giving up", tag);
    }
  } else {
    gst_tag_list_add (taglist, GST_TAG_MERGE_REPLACE,
        GST_TAG_GEO_LOCATION_NAME, name, NULL);
    offset += strlen (name);
    g_free (name);
  }

  if (len < offset + 2 + 4 + 4 + 4)
    goto short_read;

  /* +1 +1 = skip null-terminator and location role byte */
  offset += 1 + 1;
  /* table in spec says unsigned, semantics say negative has meaning ... */
  longitude = QT_SFP32 (data + offset);

  offset += 4;
  latitude = QT_SFP32 (data + offset);

  offset += 4;
  altitude = QT_SFP32 (data + offset);

  /* one invalid means all are invalid */
  if (longitude >= -180.0 && longitude <= 180.0 &&
      latitude >= -90.0 && latitude <= 90.0) {
    gst_tag_list_add (taglist, GST_TAG_MERGE_REPLACE,
        GST_TAG_GEO_LOCATION_LATITUDE, latitude,
        GST_TAG_GEO_LOCATION_LONGITUDE, longitude,
        GST_TAG_GEO_LOCATION_ELEVATION, altitude, NULL);
  }

  /* TODO: no GST_TAG_, so astronomical body and additional notes skipped */

  return;

  /* ERRORS */
short_read:
  {
    GST_DEBUG_OBJECT (qtdemux, "short read parsing 3GP location");
    return;
  }
}