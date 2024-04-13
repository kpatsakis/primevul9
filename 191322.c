qtdemux_parse_amr_bitrate (GstBuffer * buf, gboolean wb)
{
  /* The 'damr' atom is of the form:
   *
   * | vendor | decoder_ver | mode_set | mode_change_period | frames/sample |
   *    32 b       8 b          16 b           8 b                 8 b
   *
   * The highest set bit of the first 7 (AMR-NB) or 8 (AMR-WB) bits of mode_set
   * represents the highest mode used in the stream (and thus the maximum
   * bitrate), with a couple of special cases as seen below.
   */

  /* Map of frame type ID -> bitrate */
  static const guint nb_bitrates[] = {
    4750, 5150, 5900, 6700, 7400, 7950, 10200, 12200
  };
  static const guint wb_bitrates[] = {
    6600, 8850, 12650, 14250, 15850, 18250, 19850, 23050, 23850
  };
  GstMapInfo map;
  gsize max_mode;
  guint16 mode_set;

  gst_buffer_map (buf, &map, GST_MAP_READ);

  if (map.size != 0x11) {
    GST_DEBUG ("Atom should have size 0x11, not %" G_GSIZE_FORMAT, map.size);
    goto bad_data;
  }

  if (QT_FOURCC (map.data + 4) != FOURCC_damr) {
    GST_DEBUG ("Unknown atom in %" GST_FOURCC_FORMAT,
        GST_FOURCC_ARGS (QT_UINT32 (map.data + 4)));
    goto bad_data;
  }

  mode_set = QT_UINT16 (map.data + 13);

  if (mode_set == (wb ? AMR_WB_ALL_MODES : AMR_NB_ALL_MODES))
    max_mode = 7 + (wb ? 1 : 0);
  else
    /* AMR-NB modes fo from 0-7, and AMR-WB modes go from 0-8 */
    max_mode = g_bit_nth_msf ((gulong) mode_set & (wb ? 0x1ff : 0xff), -1);

  if (max_mode == -1) {
    GST_DEBUG ("No mode indication was found (mode set) = %x",
        (guint) mode_set);
    goto bad_data;
  }

  gst_buffer_unmap (buf, &map);
  return wb ? wb_bitrates[max_mode] : nb_bitrates[max_mode];

bad_data:
  gst_buffer_unmap (buf, &map);
  return 0;
}