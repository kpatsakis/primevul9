gst_aac_parse_pre_push_frame (GstBaseParse * parse, GstBaseParseFrame * frame)
{
  GstAacParse *aacparse = GST_AAC_PARSE (parse);

  if (!aacparse->sent_codec_tag) {
    GstTagList *taglist;
    GstCaps *caps;

    /* codec tag */
    caps = gst_pad_get_current_caps (GST_BASE_PARSE_SRC_PAD (parse));
    if (caps == NULL) {
      if (GST_PAD_IS_FLUSHING (GST_BASE_PARSE_SRC_PAD (parse))) {
        GST_INFO_OBJECT (parse, "Src pad is flushing");
        return GST_FLOW_FLUSHING;
      } else {
        GST_INFO_OBJECT (parse, "Src pad is not negotiated!");
        return GST_FLOW_NOT_NEGOTIATED;
      }
    }

    taglist = gst_tag_list_new_empty ();
    gst_pb_utils_add_codec_description_to_tag_list (taglist,
        GST_TAG_AUDIO_CODEC, caps);
    gst_caps_unref (caps);

    gst_base_parse_merge_tags (parse, taglist, GST_TAG_MERGE_REPLACE);
    gst_tag_list_unref (taglist);

    /* also signals the end of first-frame processing */
    aacparse->sent_codec_tag = TRUE;
  }

  /* As a special case, we can remove the ADTS framing and output raw AAC. */
  if (aacparse->header_type == DSPAAC_HEADER_ADTS
      && aacparse->output_header_type == DSPAAC_HEADER_NONE) {
    guint header_size;
    GstMapInfo map;
    frame->out_buffer = gst_buffer_make_writable (frame->buffer);
    frame->buffer = NULL;
    gst_buffer_map (frame->out_buffer, &map, GST_MAP_READ);
    header_size = (map.data[1] & 1) ? 7 : 9;    /* optional CRC */
    gst_buffer_unmap (frame->out_buffer, &map);
    gst_buffer_resize (frame->out_buffer, header_size,
        gst_buffer_get_size (frame->out_buffer) - header_size);
  }

  return GST_FLOW_OK;
}