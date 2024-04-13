static inline bool enabled_nat_bits(struct f2fs_sb_info *sbi,
					struct cp_control *cpc)
{
	bool set = is_set_ckpt_flags(sbi, CP_NAT_BITS_FLAG);

	return (cpc) ? (cpc->reason & CP_UMOUNT) && set : set;
}