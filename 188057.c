check_child_is_nk_block (hive_h *h, hive_node_h child, int flags)
{
  /* Bypass the check if flag set. */
  if (flags & GET_CHILDREN_NO_CHECK_NK)
    return 0;

  if (!IS_VALID_BLOCK (h, child)) {
    SET_ERRNO (EFAULT, "subkey is not a valid block (0x%zx)", child);
    return -1;
  }

  struct ntreg_hbin_block *block =
    (struct ntreg_hbin_block *) ((char *) h->addr + child);

  if (!block_id_eq (h, child, "nk")) {
    SET_ERRNO (EFAULT, "subkey is not an 'nk' block (0x%zx, %d, %d)",
               child, block->id[0], block->id[1]);
    return -1;
  }

  return 0;
}