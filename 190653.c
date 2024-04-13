static int f2fs_xattr_fiemap(struct inode *inode,
				struct fiemap_extent_info *fieinfo)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct page *page;
	struct node_info ni;
	__u64 phys = 0, len;
	__u32 flags;
	nid_t xnid = F2FS_I(inode)->i_xattr_nid;
	int err = 0;

	if (f2fs_has_inline_xattr(inode)) {
		int offset;

		page = f2fs_grab_cache_page(NODE_MAPPING(sbi),
						inode->i_ino, false);
		if (!page)
			return -ENOMEM;

		err = f2fs_get_node_info(sbi, inode->i_ino, &ni);
		if (err) {
			f2fs_put_page(page, 1);
			return err;
		}

		phys = (__u64)blk_to_logical(inode, ni.blk_addr);
		offset = offsetof(struct f2fs_inode, i_addr) +
					sizeof(__le32) * (DEF_ADDRS_PER_INODE -
					get_inline_xattr_addrs(inode));

		phys += offset;
		len = inline_xattr_size(inode);

		f2fs_put_page(page, 1);

		flags = FIEMAP_EXTENT_DATA_INLINE | FIEMAP_EXTENT_NOT_ALIGNED;

		if (!xnid)
			flags |= FIEMAP_EXTENT_LAST;

		err = fiemap_fill_next_extent(fieinfo, 0, phys, len, flags);
		if (err || err == 1)
			return err;
	}

	if (xnid) {
		page = f2fs_grab_cache_page(NODE_MAPPING(sbi), xnid, false);
		if (!page)
			return -ENOMEM;

		err = f2fs_get_node_info(sbi, xnid, &ni);
		if (err) {
			f2fs_put_page(page, 1);
			return err;
		}

		phys = (__u64)blk_to_logical(inode, ni.blk_addr);
		len = inode->i_sb->s_blocksize;

		f2fs_put_page(page, 1);

		flags = FIEMAP_EXTENT_LAST;
	}

	if (phys)
		err = fiemap_fill_next_extent(fieinfo, 0, phys, len, flags);

	return (err < 0 ? err : 0);
}