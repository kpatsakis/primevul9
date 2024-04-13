qtdemux_get_rtsp_uri_from_hndl (GstQTDemux * qtdemux, GNode * minf)
{
  GNode *dinf;
  GstByteReader dref;
  gchar *uri = NULL;

  /*
   * Get 'dinf', to get its child 'dref', that might contain a 'hndl'
   * atom that might contain a 'data' atom with the rtsp uri.
   * This case was reported in bug #597497, some info about
   * the hndl atom can be found in TN1195
   */
  dinf = qtdemux_tree_get_child_by_type (minf, FOURCC_dinf);
  GST_DEBUG_OBJECT (qtdemux, "Trying to obtain rtsp URI for stream trak");

  if (dinf) {
    guint32 dref_num_entries = 0;
    if (qtdemux_tree_get_child_by_type_full (dinf, FOURCC_dref, &dref) &&
        gst_byte_reader_skip (&dref, 4) &&
        gst_byte_reader_get_uint32_be (&dref, &dref_num_entries)) {
      gint i;

      /* search dref entries for hndl atom */
      for (i = 0; i < dref_num_entries; i++) {
        guint32 size = 0, type;
        guint8 string_len = 0;
        if (gst_byte_reader_get_uint32_be (&dref, &size) &&
            qt_atom_parser_get_fourcc (&dref, &type)) {
          if (type == FOURCC_hndl) {
            GST_DEBUG_OBJECT (qtdemux, "Found hndl atom");

            /* skip data reference handle bytes and the
             * following pascal string and some extra 4
             * bytes I have no idea what are */
            if (!gst_byte_reader_skip (&dref, 4) ||
                !gst_byte_reader_get_uint8 (&dref, &string_len) ||
                !gst_byte_reader_skip (&dref, string_len + 4)) {
              GST_WARNING_OBJECT (qtdemux, "Failed to parse hndl atom");
              break;
            }

            /* iterate over the atoms to find the data atom */
            while (gst_byte_reader_get_remaining (&dref) >= 8) {
              guint32 atom_size;
              guint32 atom_type;

              if (gst_byte_reader_get_uint32_be (&dref, &atom_size) &&
                  qt_atom_parser_get_fourcc (&dref, &atom_type)) {
                if (atom_type == FOURCC_data) {
                  const guint8 *uri_aux = NULL;

                  /* found the data atom that might contain the rtsp uri */
                  GST_DEBUG_OBJECT (qtdemux, "Found data atom inside "
                      "hndl atom, interpreting it as an URI");
                  if (gst_byte_reader_peek_data (&dref, atom_size - 8,
                          &uri_aux)) {
                    if (g_strstr_len ((gchar *) uri_aux, 7, "rtsp://") != NULL)
                      uri = g_strndup ((gchar *) uri_aux, atom_size - 8);
                    else
                      GST_WARNING_OBJECT (qtdemux, "Data atom in hndl atom "
                          "didn't contain a rtsp address");
                  } else {
                    GST_WARNING_OBJECT (qtdemux, "Failed to get the data "
                        "atom contents");
                  }
                  break;
                }
                /* skipping to the next entry */
                if (!gst_byte_reader_skip (&dref, atom_size - 8))
                  break;
              } else {
                GST_WARNING_OBJECT (qtdemux, "Failed to parse hndl child "
                    "atom header");
                break;
              }
            }
            break;
          }
          /* skip to the next entry */
          if (!gst_byte_reader_skip (&dref, size - 8))
            break;
        } else {
          GST_WARNING_OBJECT (qtdemux, "Error parsing dref atom");
        }
      }
      GST_DEBUG_OBJECT (qtdemux, "Finished parsing dref atom");
    }
  }
  return uri;
}