static int __allocate_data_block(struct dnode_of_data *dn, int seg_type)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(dn->inode);
	struct f2fs_summary sum;
	struct node_info ni;
	block_t old_blkaddr;
	blkcnt_t count = 1;
	int err;

	if (unlikely(is_inode_flag_set(dn->inode, FI_NO_ALLOC)))
		return -EPERM;

	err = f2fs_get_node_info(sbi, dn->nid, &ni);
	if (err)
		return err;

	dn->data_blkaddr = datablock_addr(dn->inode,
				dn->node_page, dn->ofs_in_node);
	if (dn->data_blkaddr != NULL_ADDR)
		goto alloc;

	if (unlikely((err = inc_valid_block_count(sbi, dn->inode, &count))))
		return err;

alloc:
	set_summary(&sum, dn->nid, dn->ofs_in_node, ni.version);
	old_blkaddr = dn->data_blkaddr;
	f2fs_allocate_data_block(sbi, NULL, old_blkaddr, &dn->data_blkaddr,
					&sum, seg_type, NULL, false);
	if (GET_SEGNO(sbi, old_blkaddr) != NULL_SEGNO)
		invalidate_mapping_pages(META_MAPPING(sbi),
					old_blkaddr, old_blkaddr);
	f2fs_set_data_blkaddr(dn);

	/*
	 * i_size will be updated by direct_IO. Otherwise, we'll get stale
	 * data from unwritten block via dio_read.
	 */
	return 0;
}