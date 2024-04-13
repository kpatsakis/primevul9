static inline struct address_space *NODE_MAPPING(struct f2fs_sb_info *sbi)
{
	return sbi->node_inode->i_mapping;
}