qtdemux_parse_redirects (GstQTDemux * qtdemux)
{
  GNode *rmra, *rmda, *rdrf;

  rmra = qtdemux_tree_get_child_by_type (qtdemux->moov_node, FOURCC_rmra);
  if (rmra) {
    GList *redirects = NULL;

    rmda = qtdemux_tree_get_child_by_type (rmra, FOURCC_rmda);
    while (rmda) {
      GstQtReference ref = { NULL, NULL, 0, 0 };
      GNode *rmdr, *rmvc;

      if ((rmdr = qtdemux_tree_get_child_by_type (rmda, FOURCC_rmdr))) {
        ref.min_req_bitrate = QT_UINT32 ((guint8 *) rmdr->data + 12);
        GST_LOG_OBJECT (qtdemux, "data rate atom, required bitrate = %u",
            ref.min_req_bitrate);
      }

      if ((rmvc = qtdemux_tree_get_child_by_type (rmda, FOURCC_rmvc))) {
        guint32 package = QT_FOURCC ((guint8 *) rmvc->data + 12);
        guint version = QT_UINT32 ((guint8 *) rmvc->data + 16);

#ifndef GST_DISABLE_GST_DEBUG
        guint bitmask = QT_UINT32 ((guint8 *) rmvc->data + 20);
#endif
        guint check_type = QT_UINT16 ((guint8 *) rmvc->data + 24);

        GST_LOG_OBJECT (qtdemux,
            "version check atom [%" GST_FOURCC_FORMAT "], version=0x%08x"
            ", mask=%08x, check_type=%u", GST_FOURCC_ARGS (package), version,
            bitmask, check_type);
        if (package == FOURCC_qtim && check_type == 0) {
          ref.min_req_qt_version = version;
        }
      }

      rdrf = qtdemux_tree_get_child_by_type (rmda, FOURCC_rdrf);
      if (rdrf) {
        guint32 ref_type;
        guint8 *ref_data;
        guint ref_len;

        ref_len = QT_UINT32 ((guint8 *) rdrf->data);
        if (ref_len > 20) {
          ref_type = QT_FOURCC ((guint8 *) rdrf->data + 12);
          ref_data = (guint8 *) rdrf->data + 20;
          if (ref_type == FOURCC_alis) {
            guint record_len, record_version, fn_len;

            if (ref_len > 70) {
              /* MacOSX alias record, google for alias-layout.txt */
              record_len = QT_UINT16 (ref_data + 4);
              record_version = QT_UINT16 (ref_data + 4 + 2);
              fn_len = QT_UINT8 (ref_data + 50);
              if (record_len > 50 && record_version == 2 && fn_len > 0) {
                ref.location = g_strndup ((gchar *) ref_data + 51, fn_len);
              }
            } else {
              GST_WARNING_OBJECT (qtdemux, "Invalid rdrf/alis size (%u < 70)",
                  ref_len);
            }
          } else if (ref_type == FOURCC_url_) {
            ref.location = g_strndup ((gchar *) ref_data, ref_len - 8);
          } else {
            GST_DEBUG_OBJECT (qtdemux,
                "unknown rdrf reference type %" GST_FOURCC_FORMAT,
                GST_FOURCC_ARGS (ref_type));
          }
          if (ref.location != NULL) {
            GST_INFO_OBJECT (qtdemux, "New location: %s", ref.location);
            redirects =
                g_list_prepend (redirects, g_memdup (&ref, sizeof (ref)));
          } else {
            GST_WARNING_OBJECT (qtdemux,
                "Failed to extract redirect location from rdrf atom");
          }
        } else {
          GST_WARNING_OBJECT (qtdemux, "Invalid rdrf size (%u < 20)", ref_len);
        }
      }

      /* look for others */
      rmda = qtdemux_tree_get_sibling_by_type (rmda, FOURCC_rmda);
    }

    if (redirects != NULL) {
      qtdemux_process_redirects (qtdemux, redirects);
    }
  }
  return TRUE;
}