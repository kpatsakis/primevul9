hivex_node_struct_length (hive_h *h, hive_node_h node)
{
  if (!IS_VALID_BLOCK (h, node) || !block_id_eq (h, node, "nk")) {
    SET_ERRNO (EINVAL, "invalid block or not an 'nk' block");
    return 0;
  }

  struct ntreg_nk_record *nk =
    (struct ntreg_nk_record *) ((char *) h->addr + node);
  size_t name_len = le16toh (nk->name_len);
  /* -1 to avoid double-counting the first name character */
  size_t ret = name_len + sizeof (struct ntreg_nk_record) - 1;
  int used;
  size_t seg_len = block_len (h, node, &used);
  if (ret > seg_len) {
    SET_ERRNO (EFAULT, "node name is too long (%zu, %zu)", name_len, seg_len);
    return 0;
  }
  return ret;
}