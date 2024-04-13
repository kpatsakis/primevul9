static inline struct f2fs_sb_info *F2FS_M_SB(struct address_space *mapping)
{
	return F2FS_I_SB(mapping->host);
}