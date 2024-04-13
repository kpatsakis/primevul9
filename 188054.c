hivex_node_children (hive_h *h, hive_node_h node)
{
  hive_node_h *children;
  size_t *blocks;

  if (_hivex_get_children (h, node, &children, &blocks, 0) == -1)
    return NULL;

  free (blocks);
  return children;
}