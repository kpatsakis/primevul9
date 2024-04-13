gst_matroska_decode_buffer (GstMatroskaTrackContext * context, GstBuffer * buf)
{
  GstMapInfo map;
  gpointer data;
  gsize size;
  GstBuffer *out_buf = buf;

  g_return_val_if_fail (GST_IS_BUFFER (buf), NULL);

  GST_DEBUG ("decoding buffer %p", buf);

  gst_buffer_map (out_buf, &map, GST_MAP_READ);
  data = map.data;
  size = map.size;

  g_return_val_if_fail (size > 0, buf);

  if (gst_matroska_decode_data (context->encodings, &data, &size,
          GST_MATROSKA_TRACK_ENCODING_SCOPE_FRAME, FALSE)) {
    if (data != map.data) {
      gst_buffer_unmap (out_buf, &map);
      gst_buffer_unref (out_buf);
      out_buf = gst_buffer_new_wrapped (data, size);
    } else {
      gst_buffer_unmap (out_buf, &map);
    }
  } else {
    GST_DEBUG ("decode data failed");
    gst_buffer_unmap (out_buf, &map);
    gst_buffer_unref (out_buf);
    return NULL;
  }
  /* Encrypted stream */
  if (context->protection_info) {

    GstStructure *info_protect = gst_structure_copy (context->protection_info);
    gboolean encrypted = FALSE;

    gst_buffer_map (out_buf, &map, GST_MAP_READ);
    data = map.data;
    size = map.size;

    if (gst_matroska_parse_protection_meta (&data, &size, info_protect,
            &encrypted)) {
      if (data != map.data) {
        GstBuffer *tmp_buf;

        gst_buffer_unmap (out_buf, &map);
        tmp_buf = out_buf;
        out_buf = gst_buffer_copy_region (tmp_buf, GST_BUFFER_COPY_ALL,
            gst_buffer_get_size (tmp_buf) - size, size);
        gst_buffer_unref (tmp_buf);
        if (encrypted)
          gst_buffer_add_protection_meta (out_buf, info_protect);
        else
          gst_structure_free (info_protect);
      } else {
        gst_buffer_unmap (out_buf, &map);
        gst_structure_free (info_protect);
      }
    } else {
      GST_WARNING ("Adding protection metadata failed");
      gst_buffer_unmap (out_buf, &map);
      gst_buffer_unref (out_buf);
      gst_structure_free (info_protect);
      return NULL;
    }
  }

  return out_buf;
}