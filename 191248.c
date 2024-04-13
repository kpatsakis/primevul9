qtdemux_add_container_format (GstQTDemux * qtdemux, GstTagList * tags)
{
  const gchar *fmt;

  if (tags == NULL) {
    tags = gst_tag_list_new_empty ();
    gst_tag_list_set_scope (tags, GST_TAG_SCOPE_GLOBAL);
  }

  if (qtdemux->major_brand == FOURCC_mjp2)
    fmt = "Motion JPEG 2000";
  else if ((qtdemux->major_brand & 0xffff) == FOURCC_3g__)
    fmt = "3GP";
  else if (qtdemux->major_brand == FOURCC_qt__)
    fmt = "Quicktime";
  else if (qtdemux->fragmented)
    fmt = "ISO fMP4";
  else
    fmt = "ISO MP4/M4A";

  GST_LOG_OBJECT (qtdemux, "mapped %" GST_FOURCC_FORMAT " to '%s'",
      GST_FOURCC_ARGS (qtdemux->major_brand), fmt);

  gst_tag_list_add (tags, GST_TAG_MERGE_REPLACE, GST_TAG_CONTAINER_FORMAT,
      fmt, NULL);

  return tags;
}