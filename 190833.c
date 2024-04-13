gst_aac_parse_sink_getcaps (GstBaseParse * parse, GstCaps * filter)
{
  GstCaps *peercaps, *templ;
  GstCaps *res;

  templ = gst_pad_get_pad_template_caps (GST_BASE_PARSE_SINK_PAD (parse));

  if (filter) {
    GstCaps *fcopy = gst_caps_copy (filter);
    /* Remove the fields we convert */
    remove_fields (fcopy);
    add_conversion_fields (fcopy);
    peercaps = gst_pad_peer_query_caps (GST_BASE_PARSE_SRC_PAD (parse), fcopy);
    gst_caps_unref (fcopy);
  } else
    peercaps = gst_pad_peer_query_caps (GST_BASE_PARSE_SRC_PAD (parse), NULL);

  if (peercaps) {
    peercaps = gst_caps_make_writable (peercaps);
    /* Remove the fields we convert */
    remove_fields (peercaps);
    add_conversion_fields (peercaps);

    res = gst_caps_intersect_full (peercaps, templ, GST_CAPS_INTERSECT_FIRST);
    gst_caps_unref (peercaps);
    gst_caps_unref (templ);
  } else {
    res = templ;
  }

  if (filter) {
    GstCaps *intersection;

    intersection =
        gst_caps_intersect_full (filter, res, GST_CAPS_INTERSECT_FIRST);
    gst_caps_unref (res);
    res = intersection;
  }

  return res;
}