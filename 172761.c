static void ext4_set_iomap(struct inode *inode, struct iomap *iomap,
			   struct ext4_map_blocks *map, loff_t offset,
			   loff_t length)
{
	u8 blkbits = inode->i_blkbits;

	/*
	 * Writes that span EOF might trigger an I/O size update on completion,
	 * so consider them to be dirty for the purpose of O_DSYNC, even if
	 * there is no other metadata changes being made or are pending.
	 */
	iomap->flags = 0;
	if (ext4_inode_datasync_dirty(inode) ||
	    offset + length > i_size_read(inode))
		iomap->flags |= IOMAP_F_DIRTY;

	if (map->m_flags & EXT4_MAP_NEW)
		iomap->flags |= IOMAP_F_NEW;

	iomap->bdev = inode->i_sb->s_bdev;
	iomap->dax_dev = EXT4_SB(inode->i_sb)->s_daxdev;
	iomap->offset = (u64) map->m_lblk << blkbits;
	iomap->length = (u64) map->m_len << blkbits;

	/*
	 * Flags passed to ext4_map_blocks() for direct I/O writes can result
	 * in m_flags having both EXT4_MAP_MAPPED and EXT4_MAP_UNWRITTEN bits
	 * set. In order for any allocated unwritten extents to be converted
	 * into written extents correctly within the ->end_io() handler, we
	 * need to ensure that the iomap->type is set appropriately. Hence, the
	 * reason why we need to check whether the EXT4_MAP_UNWRITTEN bit has
	 * been set first.
	 */
	if (map->m_flags & EXT4_MAP_UNWRITTEN) {
		iomap->type = IOMAP_UNWRITTEN;
		iomap->addr = (u64) map->m_pblk << blkbits;
	} else if (map->m_flags & EXT4_MAP_MAPPED) {
		iomap->type = IOMAP_MAPPED;
		iomap->addr = (u64) map->m_pblk << blkbits;
	} else {
		iomap->type = IOMAP_HOLE;
		iomap->addr = IOMAP_NULL_ADDR;
	}
}