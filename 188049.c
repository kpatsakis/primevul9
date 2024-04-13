hivex_node_name (hive_h *h, hive_node_h node)
{
  if (!IS_VALID_BLOCK (h, node) || !block_id_eq (h, node, "nk")) {
    SET_ERRNO (EINVAL, "invalid block or not an 'nk' block");
    return NULL;
  }

  struct ntreg_nk_record *nk =
    (struct ntreg_nk_record *) ((char *) h->addr + node);

  /* nk->name_len is unsigned, 16 bit, so this is safe ...  However
   * we have to make sure the length doesn't exceed the block length.
   */
  size_t len = le16toh (nk->name_len);
  size_t seg_len = block_len (h, node, NULL);
  if (sizeof (struct ntreg_nk_record) + len - 1 > seg_len) {
    SET_ERRNO (EFAULT, "node name is too long (%zu, %zu)", len, seg_len);
    return NULL;
  }
  size_t flags = le16toh (nk->flags);
  if (flags & 0x20) {
    return _hivex_recode (h, latin1_to_utf8, nk->name, len, NULL);
  } else {
    return _hivex_recode (h, utf16le_to_utf8, nk->name, len, NULL);
  }
}