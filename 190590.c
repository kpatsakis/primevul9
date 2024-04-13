

static inline void set_opt_mode(struct f2fs_sb_info *sbi, unsigned int mt)
{
	clear_opt(sbi, ADAPTIVE);
	clear_opt(sbi, LFS);

	switch (mt) {
	case F2FS_MOUNT_ADAPTIVE:
		set_opt(sbi, ADAPTIVE);
		break;
	case F2FS_MOUNT_LFS:
		set_opt(sbi, LFS);
		break;