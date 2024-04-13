qtdemux_parse_uuid (GstQTDemux * qtdemux, const guint8 * buffer, gint length)
{
  static const guint8 xmp_uuid[] = { 0xBE, 0x7A, 0xCF, 0xCB,
    0x97, 0xA9, 0x42, 0xE8,
    0x9C, 0x71, 0x99, 0x94,
    0x91, 0xE3, 0xAF, 0xAC
  };
  static const guint8 playready_uuid[] = {
    0xd0, 0x8a, 0x4f, 0x18, 0x10, 0xf3, 0x4a, 0x82,
    0xb6, 0xc8, 0x32, 0xd8, 0xab, 0xa1, 0x83, 0xd3
  };

  static const guint8 piff_sample_encryption_uuid[] = {
    0xa2, 0x39, 0x4f, 0x52, 0x5a, 0x9b, 0x4f, 0x14,
    0xa2, 0x44, 0x6c, 0x42, 0x7c, 0x64, 0x8d, 0xf4
  };

  guint offset;

  /* counts as header data */
  qtdemux->header_size += length;

  offset = (QT_UINT32 (buffer) == 0) ? 16 : 8;

  if (length <= offset + 16) {
    GST_DEBUG_OBJECT (qtdemux, "uuid atom is too short, skipping");
    return;
  }

  if (memcmp (buffer + offset, xmp_uuid, 16) == 0) {
    GstBuffer *buf;
    GstTagList *taglist;

    buf = _gst_buffer_new_wrapped ((guint8 *) buffer + offset + 16,
        length - offset - 16, NULL);
    taglist = gst_tag_list_from_xmp_buffer (buf);
    gst_buffer_unref (buf);

    qtdemux_handle_xmp_taglist (qtdemux, qtdemux->tag_list, taglist);

  } else if (memcmp (buffer + offset, playready_uuid, 16) == 0) {
    int len;
    const gunichar2 *s_utf16;
    char *contents;

    len = GST_READ_UINT16_LE (buffer + offset + 0x30);
    s_utf16 = (const gunichar2 *) (buffer + offset + 0x32);
    contents = g_utf16_to_utf8 (s_utf16, len / 2, NULL, NULL, NULL);
    GST_ERROR_OBJECT (qtdemux, "contents: %s", contents);

    g_free (contents);

    GST_ELEMENT_ERROR (qtdemux, STREAM, DECRYPT,
        (_("Cannot play stream because it is encrypted with PlayReady DRM.")),
        (NULL));
  } else if (memcmp (buffer + offset, piff_sample_encryption_uuid, 16) == 0) {
    qtdemux_parse_piff (qtdemux, buffer, length, offset);
  } else {
    GST_DEBUG_OBJECT (qtdemux, "Ignoring unknown uuid: %08x-%08x-%08x-%08x",
        GST_READ_UINT32_LE (buffer + offset),
        GST_READ_UINT32_LE (buffer + offset + 4),
        GST_READ_UINT32_LE (buffer + offset + 8),
        GST_READ_UINT32_LE (buffer + offset + 12));
  }
}