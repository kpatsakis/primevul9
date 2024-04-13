static inline struct address_space *META_MAPPING(struct f2fs_sb_info *sbi)
{
	return sbi->meta_inode->i_mapping;
}