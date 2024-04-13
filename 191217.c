qtdemux_tag_add_str (GstQTDemux * qtdemux, GstTagList * taglist,
    const char *tag, const char *dummy, GNode * node)
{
  qtdemux_tag_add_str_full (qtdemux, taglist, tag, dummy, node);
}