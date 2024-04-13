static int iwl_pcie_load_cpu_sections_8000(struct iwl_trans *trans,
					   const struct fw_img *image,
					   int cpu,
					   int *first_ucode_section)
{
	int shift_param;
	int i, ret = 0, sec_num = 0x1;
	u32 val, last_read_idx = 0;

	if (cpu == 1) {
		shift_param = 0;
		*first_ucode_section = 0;
	} else {
		shift_param = 16;
		(*first_ucode_section)++;
	}

	for (i = *first_ucode_section; i < image->num_sec; i++) {
		last_read_idx = i;

		/*
		 * CPU1_CPU2_SEPARATOR_SECTION delimiter - separate between
		 * CPU1 to CPU2.
		 * PAGING_SEPARATOR_SECTION delimiter - separate between
		 * CPU2 non paged to CPU2 paging sec.
		 */
		if (!image->sec[i].data ||
		    image->sec[i].offset == CPU1_CPU2_SEPARATOR_SECTION ||
		    image->sec[i].offset == PAGING_SEPARATOR_SECTION) {
			IWL_DEBUG_FW(trans,
				     "Break since Data not valid or Empty section, sec = %d\n",
				     i);
			break;
		}

		ret = iwl_pcie_load_section(trans, i, &image->sec[i]);
		if (ret)
			return ret;

		/* Notify ucode of loaded section number and status */
		val = iwl_read_direct32(trans, FH_UCODE_LOAD_STATUS);
		val = val | (sec_num << shift_param);
		iwl_write_direct32(trans, FH_UCODE_LOAD_STATUS, val);

		sec_num = (sec_num << 1) | 0x1;
	}

	*first_ucode_section = last_read_idx;

	iwl_enable_interrupts(trans);

	if (trans->trans_cfg->use_tfh) {
		if (cpu == 1)
			iwl_write_prph(trans, UREG_UCODE_LOAD_STATUS,
				       0xFFFF);
		else
			iwl_write_prph(trans, UREG_UCODE_LOAD_STATUS,
				       0xFFFFFFFF);
	} else {
		if (cpu == 1)
			iwl_write_direct32(trans, FH_UCODE_LOAD_STATUS,
					   0xFFFF);
		else
			iwl_write_direct32(trans, FH_UCODE_LOAD_STATUS,
					   0xFFFFFFFF);
	}

	return 0;
}