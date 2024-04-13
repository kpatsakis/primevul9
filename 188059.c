hivex_node_security (hive_h *h, hive_node_h node)
{
  if (!IS_VALID_BLOCK (h, node) || !block_id_eq (h, node, "nk")) {
    SET_ERRNO (EINVAL, "invalid block or not an 'nk' block");
    return 0;
  }

  struct ntreg_nk_record *nk = (struct ntreg_nk_record *) (h->addr + node);

  hive_node_h ret = le32toh (nk->sk);
  ret += 0x1000;
  if (!IS_VALID_BLOCK (h, ret)) {
    SET_ERRNO (EFAULT, "invalid block");
    return 0;
  }
  return ret;
}