static int bnx2x_get_igu_cam_info(struct bnx2x *bp)
{
	int pfid = BP_FUNC(bp);
	int igu_sb_id;
	u32 val;
	u8 fid, igu_sb_cnt = 0;

	bp->igu_base_sb = 0xff;
	if (CHIP_INT_MODE_IS_BC(bp)) {
		int vn = BP_VN(bp);
		igu_sb_cnt = bp->igu_sb_cnt;
		bp->igu_base_sb = (CHIP_MODE_IS_4_PORT(bp) ? pfid : vn) *
			FP_SB_MAX_E1x;

		bp->igu_dsb_id =  E1HVN_MAX * FP_SB_MAX_E1x +
			(CHIP_MODE_IS_4_PORT(bp) ? pfid : vn);

		return 0;
	}

	/* IGU in normal mode - read CAM */
	for (igu_sb_id = 0; igu_sb_id < IGU_REG_MAPPING_MEMORY_SIZE;
	     igu_sb_id++) {
		val = REG_RD(bp, IGU_REG_MAPPING_MEMORY + igu_sb_id * 4);
		if (!(val & IGU_REG_MAPPING_MEMORY_VALID))
			continue;
		fid = IGU_FID(val);
		if ((fid & IGU_FID_ENCODE_IS_PF)) {
			if ((fid & IGU_FID_PF_NUM_MASK) != pfid)
				continue;
			if (IGU_VEC(val) == 0)
				/* default status block */
				bp->igu_dsb_id = igu_sb_id;
			else {
				if (bp->igu_base_sb == 0xff)
					bp->igu_base_sb = igu_sb_id;
				igu_sb_cnt++;
			}
		}
	}

#ifdef CONFIG_PCI_MSI
	/* Due to new PF resource allocation by MFW T7.4 and above, it's
	 * optional that number of CAM entries will not be equal to the value
	 * advertised in PCI.
	 * Driver should use the minimal value of both as the actual status
	 * block count
	 */
	bp->igu_sb_cnt = min_t(int, bp->igu_sb_cnt, igu_sb_cnt);
#endif

	if (igu_sb_cnt == 0) {
		BNX2X_ERR("CAM configuration error\n");
		return -EINVAL;
	}

	return 0;
}