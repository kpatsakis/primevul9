qtdemux_tag_add_revdns (GstQTDemux * demux, GstTagList * taglist,
    const char *tag, const char *tag_bis, GNode * node)
{
  GNode *mean;
  GNode *name;
  GNode *data;
  guint32 meansize;
  guint32 namesize;
  guint32 datatype;
  guint32 datasize;
  const gchar *meanstr;
  const gchar *namestr;

  /* checking the whole ---- atom size for consistency */
  if (QT_UINT32 (node->data) <= 4 + 12 + 12 + 16) {
    GST_WARNING_OBJECT (demux, "Tag ---- atom is too small, ignoring");
    return;
  }

  mean = qtdemux_tree_get_child_by_type (node, FOURCC_mean);
  if (!mean) {
    GST_WARNING_OBJECT (demux, "No 'mean' atom found");
    return;
  }

  meansize = QT_UINT32 (mean->data);
  if (meansize <= 12) {
    GST_WARNING_OBJECT (demux, "Small mean atom, ignoring the whole tag");
    return;
  }
  meanstr = ((gchar *) mean->data) + 12;
  meansize -= 12;

  name = qtdemux_tree_get_child_by_type (node, FOURCC_name);
  if (!name) {
    GST_WARNING_OBJECT (demux, "'name' atom not found, ignoring tag");
    return;
  }

  namesize = QT_UINT32 (name->data);
  if (namesize <= 12) {
    GST_WARNING_OBJECT (demux, "'name' atom is too small, ignoring tag");
    return;
  }
  namestr = ((gchar *) name->data) + 12;
  namesize -= 12;

  /*
   * Data atom is:
   * uint32 - size
   * uint32 - name
   * uint8  - version
   * uint24 - data type
   * uint32 - all 0
   * rest   - the data
   */
  data = qtdemux_tree_get_child_by_type (node, FOURCC_data);
  if (!data) {
    GST_WARNING_OBJECT (demux, "No data atom in this tag");
    return;
  }
  datasize = QT_UINT32 (data->data);
  if (datasize <= 16) {
    GST_WARNING_OBJECT (demux, "Data atom too small");
    return;
  }
  datatype = QT_UINT32 (((gchar *) data->data) + 8) & 0xFFFFFF;

  if ((strncmp (meanstr, "com.apple.iTunes", meansize) == 0) ||
      (strncmp (meanstr, "org.hydrogenaudio.replaygain", meansize) == 0)) {
    static const struct
    {
      const gchar name[28];
      const gchar tag[28];
    } tags[] = {
      {
      "replaygain_track_gain", GST_TAG_TRACK_GAIN}, {
      "replaygain_track_peak", GST_TAG_TRACK_PEAK}, {
      "replaygain_album_gain", GST_TAG_ALBUM_GAIN}, {
      "replaygain_album_peak", GST_TAG_ALBUM_PEAK}, {
      "MusicBrainz Track Id", GST_TAG_MUSICBRAINZ_TRACKID}, {
      "MusicBrainz Artist Id", GST_TAG_MUSICBRAINZ_ARTISTID}, {
      "MusicBrainz Album Id", GST_TAG_MUSICBRAINZ_ALBUMID}, {
      "MusicBrainz Album Artist Id", GST_TAG_MUSICBRAINZ_ALBUMARTISTID}
    };
    int i;

    for (i = 0; i < G_N_ELEMENTS (tags); ++i) {
      if (!g_ascii_strncasecmp (tags[i].name, namestr, namesize)) {
        switch (gst_tag_get_type (tags[i].tag)) {
          case G_TYPE_DOUBLE:
            qtdemux_add_double_tag_from_str (demux, taglist, tags[i].tag,
                ((guint8 *) data->data) + 16, datasize - 16);
            break;
          case G_TYPE_STRING:
            qtdemux_tag_add_str (demux, taglist, tags[i].tag, NULL, node);
            break;
          default:
            /* not reached */
            break;
        }
        break;
      }
    }
    if (i == G_N_ELEMENTS (tags))
      goto unknown_tag;
  } else {
    goto unknown_tag;
  }

  return;

/* errors */
unknown_tag:
#ifndef GST_DISABLE_GST_DEBUG
  {
    gchar *namestr_dbg;
    gchar *meanstr_dbg;

    meanstr_dbg = g_strndup (meanstr, meansize);
    namestr_dbg = g_strndup (namestr, namesize);

    GST_WARNING_OBJECT (demux, "This tag %s:%s type:%u is not mapped, "
        "file a bug at bugzilla.gnome.org", meanstr_dbg, namestr_dbg, datatype);

    g_free (namestr_dbg);
    g_free (meanstr_dbg);
  }
#endif
  return;
}