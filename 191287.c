qtdemux_tree_get_sibling_by_type (GNode * node, guint32 fourcc)
{
  return qtdemux_tree_get_sibling_by_type_full (node, fourcc, NULL);
}