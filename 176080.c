static inline bool sanity_check_area_boundary(struct super_block *sb,
					struct f2fs_super_block *raw_super)
{
	u32 segment0_blkaddr = le32_to_cpu(raw_super->segment0_blkaddr);
	u32 cp_blkaddr = le32_to_cpu(raw_super->cp_blkaddr);
	u32 sit_blkaddr = le32_to_cpu(raw_super->sit_blkaddr);
	u32 nat_blkaddr = le32_to_cpu(raw_super->nat_blkaddr);
	u32 ssa_blkaddr = le32_to_cpu(raw_super->ssa_blkaddr);
	u32 main_blkaddr = le32_to_cpu(raw_super->main_blkaddr);
	u32 segment_count_ckpt = le32_to_cpu(raw_super->segment_count_ckpt);
	u32 segment_count_sit = le32_to_cpu(raw_super->segment_count_sit);
	u32 segment_count_nat = le32_to_cpu(raw_super->segment_count_nat);
	u32 segment_count_ssa = le32_to_cpu(raw_super->segment_count_ssa);
	u32 segment_count_main = le32_to_cpu(raw_super->segment_count_main);
	u32 segment_count = le32_to_cpu(raw_super->segment_count);
	u32 log_blocks_per_seg = le32_to_cpu(raw_super->log_blocks_per_seg);

	if (segment0_blkaddr != cp_blkaddr) {
		f2fs_msg(sb, KERN_INFO,
			"Mismatch start address, segment0(%u) cp_blkaddr(%u)",
			segment0_blkaddr, cp_blkaddr);
		return true;
	}

	if (cp_blkaddr + (segment_count_ckpt << log_blocks_per_seg) !=
							sit_blkaddr) {
		f2fs_msg(sb, KERN_INFO,
			"Wrong CP boundary, start(%u) end(%u) blocks(%u)",
			cp_blkaddr, sit_blkaddr,
			segment_count_ckpt << log_blocks_per_seg);
		return true;
	}

	if (sit_blkaddr + (segment_count_sit << log_blocks_per_seg) !=
							nat_blkaddr) {
		f2fs_msg(sb, KERN_INFO,
			"Wrong SIT boundary, start(%u) end(%u) blocks(%u)",
			sit_blkaddr, nat_blkaddr,
			segment_count_sit << log_blocks_per_seg);
		return true;
	}

	if (nat_blkaddr + (segment_count_nat << log_blocks_per_seg) !=
							ssa_blkaddr) {
		f2fs_msg(sb, KERN_INFO,
			"Wrong NAT boundary, start(%u) end(%u) blocks(%u)",
			nat_blkaddr, ssa_blkaddr,
			segment_count_nat << log_blocks_per_seg);
		return true;
	}

	if (ssa_blkaddr + (segment_count_ssa << log_blocks_per_seg) !=
							main_blkaddr) {
		f2fs_msg(sb, KERN_INFO,
			"Wrong SSA boundary, start(%u) end(%u) blocks(%u)",
			ssa_blkaddr, main_blkaddr,
			segment_count_ssa << log_blocks_per_seg);
		return true;
	}

	if (main_blkaddr + (segment_count_main << log_blocks_per_seg) !=
		segment0_blkaddr + (segment_count << log_blocks_per_seg)) {
		f2fs_msg(sb, KERN_INFO,
			"Wrong MAIN_AREA boundary, start(%u) end(%u) blocks(%u)",
			main_blkaddr,
			segment0_blkaddr + (segment_count << log_blocks_per_seg),
			segment_count_main << log_blocks_per_seg);
		return true;
	}

	return false;
}