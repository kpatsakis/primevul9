static void ext4_xattr_block_csum_set(struct inode *inode,
				      struct buffer_head *bh)
{
	if (ext4_has_metadata_csum(inode->i_sb))
		BHDR(bh)->h_checksum = ext4_xattr_block_csum(inode,
						bh->b_blocknr, BHDR(bh));
}