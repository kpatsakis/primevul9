hivex_node_nr_children (hive_h *h, hive_node_h node)
{
  if (!IS_VALID_BLOCK (h, node) || !block_id_eq (h, node, "nk")) {
    SET_ERRNO( EINVAL, "invalid block or not an 'nk' block");
    return 0;
  }

  struct ntreg_nk_record *nk =
    (struct ntreg_nk_record *) ((char *) h->addr + node);

  size_t nr_subkeys_in_nk = le32toh (nk->nr_subkeys);

  return nr_subkeys_in_nk;
}