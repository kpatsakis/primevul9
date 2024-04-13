static inline size_t round_up_cluster(struct inode *inode, size_t length)
{
	struct super_block *sb = inode->i_sb;
	size_t cluster_size = 1 << (EXT4_SB(sb)->s_cluster_bits +
				    inode->i_blkbits);
	size_t mask = ~(cluster_size - 1);

	return (length + cluster_size - 1) & mask;
}