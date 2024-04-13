int f2fs_reserve_new_blocks(struct dnode_of_data *dn, blkcnt_t count)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(dn->inode);
	int err;

	if (!count)
		return 0;

	if (unlikely(is_inode_flag_set(dn->inode, FI_NO_ALLOC)))
		return -EPERM;
	if (unlikely((err = inc_valid_block_count(sbi, dn->inode, &count))))
		return err;

	trace_f2fs_reserve_new_blocks(dn->inode, dn->nid,
						dn->ofs_in_node, count);

	f2fs_wait_on_page_writeback(dn->node_page, NODE, true, true);

	for (; count > 0; dn->ofs_in_node++) {
		block_t blkaddr = datablock_addr(dn->inode,
					dn->node_page, dn->ofs_in_node);
		if (blkaddr == NULL_ADDR) {
			dn->data_blkaddr = NEW_ADDR;
			__set_data_blkaddr(dn);
			count--;
		}
	}

	if (set_page_dirty(dn->node_page))
		dn->node_changed = true;
	return 0;
}