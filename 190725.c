static inline block_t datablock_addr(struct inode *inode,
			struct page *node_page, unsigned int offset)
{
	struct f2fs_node *raw_node;
	__le32 *addr_array;
	int base = 0;
	bool is_inode = IS_INODE(node_page);

	raw_node = F2FS_NODE(node_page);

	/* from GC path only */
	if (is_inode) {
		if (!inode)
			base = offset_in_addr(&raw_node->i);
		else if (f2fs_has_extra_attr(inode))
			base = get_extra_isize(inode);
	}

	addr_array = blkaddr_in_node(raw_node);
	return le32_to_cpu(addr_array[base + offset]);
}