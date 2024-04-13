hivex_node_timestamp (hive_h *h, hive_node_h node)
{
  int64_t ret;

  if (!IS_VALID_BLOCK (h, node) || !block_id_eq (h, node, "nk")) {
    SET_ERRNO (EINVAL, "invalid block or not an 'nk' block");
    return -1;
  }

  struct ntreg_nk_record *nk =
    (struct ntreg_nk_record *) ((char *) h->addr + node);

  ret = le64toh (nk->timestamp);
  return timestamp_check (h, node, ret);
}