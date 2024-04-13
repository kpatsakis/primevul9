hivex_root (hive_h *h)
{
  hive_node_h ret = h->rootoffs;
  if (!IS_VALID_BLOCK (h, ret)) {
    SET_ERRNO (HIVEX_NO_KEY, "no root key");
    return 0;
  }
  return ret;
}