qtdemux_handle_xmp_taglist (GstQTDemux * qtdemux, GstTagList * taglist,
    GstTagList * xmptaglist)
{
  /* Strip out bogus fields */
  if (xmptaglist) {
    if (gst_tag_list_get_scope (taglist) == GST_TAG_SCOPE_GLOBAL) {
      gst_tag_list_remove_tag (xmptaglist, GST_TAG_VIDEO_CODEC);
      gst_tag_list_remove_tag (xmptaglist, GST_TAG_AUDIO_CODEC);
    } else {
      gst_tag_list_remove_tag (xmptaglist, GST_TAG_CONTAINER_FORMAT);
    }

    GST_DEBUG_OBJECT (qtdemux, "Found XMP tags %" GST_PTR_FORMAT, xmptaglist);

    /* prioritize native tags using _KEEP mode */
    gst_tag_list_insert (taglist, xmptaglist, GST_TAG_MERGE_KEEP);
    gst_tag_list_unref (xmptaglist);
  }
}