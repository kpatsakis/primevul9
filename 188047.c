_get_children (hive_h *h, hive_node_h blkoff,
               offset_list *children, offset_list *blocks,
               int flags, unsigned depth)
{
  /* Add this intermediate block. */
  if (_hivex_add_to_offset_list (blocks, blkoff) == -1)
    return -1;

  struct ntreg_hbin_block *block =
    (struct ntreg_hbin_block *) ((char *) h->addr + blkoff);

  size_t len = block_len (h, blkoff, NULL);

  /* Points to lf-record?  (Note, also "lh" but that is basically the
   * same as "lf" as far as we are concerned here).
   */
  if (block->id[0] == 'l' && (block->id[1] == 'f' || block->id[1] == 'h')) {
    struct ntreg_lf_record *lf = (struct ntreg_lf_record *) block;

    /* Check number of subkeys in the nk-record matches number of subkeys
     * in the lf-record.
     */
    size_t nr_subkeys_in_lf = le16toh (lf->nr_keys);

    if (8 + nr_subkeys_in_lf * 8 > len) {
      SET_ERRNO (EFAULT, "too many subkeys (%zu, %zu)", nr_subkeys_in_lf, len);
      return -1;
    }

    size_t i;
    for (i = 0; i < nr_subkeys_in_lf; ++i) {
      hive_node_h subkey = le32toh (lf->keys[i].offset);
      subkey += 0x1000;
      if (check_child_is_nk_block (h, subkey, flags) == -1) {
        if (h->unsafe) {
          DEBUG (2, "subkey at 0x%zx is not an NK block, skipping", subkey);
          continue;
        } else {
          return -1;
        }
      }
      if (_hivex_add_to_offset_list (children, subkey) == -1)
        return -1;
    }
  }
  /* Points to li-record? */
  else if (block->id[0] == 'l' && block->id[1] == 'i') {
    /* li-records are formatted the same as ri-records, but they
     * contain direct links to child records (same as lf/lh), so
     * we treat them the same way as lf/lh.
     */
    struct ntreg_ri_record *ri = (struct ntreg_ri_record *) block;

    /* Check number of subkeys in the nk-record matches number of subkeys
     * in the li-record.
     */
    size_t nr_offsets = le16toh (ri->nr_offsets);

    if (8 + nr_offsets * 4 > len) {
      SET_ERRNO (EFAULT, "too many offsets (%zu, %zu)", nr_offsets, len);
      return -1;
    }

    size_t i;
    for (i = 0; i < nr_offsets; ++i) {
      hive_node_h subkey = le32toh (ri->offset[i]);
      subkey += 0x1000;
      if (check_child_is_nk_block (h, subkey, flags) == -1) {
        if (h->unsafe) {
          DEBUG (2, "subkey at 0x%zx is not an NK block, skipping", subkey);
          continue;
        } else {
          return -1;
        }
      }
      if (_hivex_add_to_offset_list (children, subkey) == -1)
        return -1;
    }
  }
  /* Points to ri-record? */
  else if (block->id[0] == 'r' && block->id[1] == 'i') {
    struct ntreg_ri_record *ri = (struct ntreg_ri_record *) block;

    size_t nr_offsets = le16toh (ri->nr_offsets);

    if (8 + nr_offsets * 4 > len) {
      SET_ERRNO (EFAULT, "too many offsets (%zu, %zu)", nr_offsets, len);
      return -1;
    }

    /* Copy list of children. */
    size_t i;
    for (i = 0; i < nr_offsets; ++i) {
      hive_node_h offset = le32toh (ri->offset[i]);
      offset += 0x1000;
      if (!IS_VALID_BLOCK (h, offset)) {
        if (h->unsafe) {
          DEBUG (2, "ri-offset is not a valid block (0x%zx), skipping", offset);
          continue;
        } else {
          SET_ERRNO (EFAULT, "ri-offset is not a valid block (0x%zx)", offset);
          return -1;
        }
      }

      /* Although in theory hive ri records might be nested to any
       * depth, in practice this is unlikely.  Recursing here caused
       * CVE-2021-3622.  Thus limit the depth we will recurse to
       * something small.
       */
      if (depth >= 32) {
        SET_ERRNO (EINVAL, "ri-record nested to depth >= %u", depth);
        return -1;
      }

      if (_get_children (h, offset, children, blocks, flags, depth+1) == -1)
        return -1;
    }
  }
  else {
    SET_ERRNO (ENOTSUP,
               "subkey block is not lf/lh/li/ri (0x%zx, %d, %d)",
               blkoff, block->id[0], block->id[1]);
    return -1;
  }

  return 0;
}