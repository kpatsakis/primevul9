gst_aac_parse_get_audio_profile_object_type (GstAacParse * aacparse)
{
  GstCaps *srccaps;
  GstStructure *srcstruct;
  const gchar *profile;
  guint8 ret;

  srccaps = gst_pad_get_current_caps (GST_BASE_PARSE_SRC_PAD (aacparse));
  if (G_UNLIKELY (srccaps == NULL)) {
    return G_MAXUINT8;
  }

  srcstruct = gst_caps_get_structure (srccaps, 0);
  profile = gst_structure_get_string (srcstruct, "profile");
  if (G_UNLIKELY (profile == NULL)) {
    gst_caps_unref (srccaps);
    return G_MAXUINT8;
  }

  if (g_strcmp0 (profile, "main") == 0) {
    ret = (guint8) 0U;
  } else if (g_strcmp0 (profile, "lc") == 0) {
    ret = (guint8) 1U;
  } else if (g_strcmp0 (profile, "ssr") == 0) {
    ret = (guint8) 2U;
  } else if (g_strcmp0 (profile, "ltp") == 0) {
    if (G_LIKELY (aacparse->mpegversion == 4))
      ret = (guint8) 3U;
    else
      ret = G_MAXUINT8;         /* LTP Object Type allowed only for MPEG-4 */
  } else {
    ret = G_MAXUINT8;
  }

  gst_caps_unref (srccaps);
  return ret;
}