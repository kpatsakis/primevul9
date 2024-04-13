static bool bnx2x_check_blocks_with_parity1(struct bnx2x *bp, u32 sig,
					    int *par_num, bool *global,
					    bool print)
{
	u32 cur_bit;
	bool res;
	int i;

	res = false;

	for (i = 0; sig; i++) {
		cur_bit = (0x1UL << i);
		if (sig & cur_bit) {
			res |= true; /* Each bit is real error! */
			switch (cur_bit) {
			case AEU_INPUTS_ATTN_BITS_PBF_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++, "PBF");
					_print_parity(bp, PBF_REG_PBF_PRTY_STS);
				}
				break;
			case AEU_INPUTS_ATTN_BITS_QM_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++, "QM");
					_print_parity(bp, QM_REG_QM_PRTY_STS);
				}
				break;
			case AEU_INPUTS_ATTN_BITS_TIMERS_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++, "TM");
					_print_parity(bp, TM_REG_TM_PRTY_STS);
				}
				break;
			case AEU_INPUTS_ATTN_BITS_XSDM_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++, "XSDM");
					_print_parity(bp,
						      XSDM_REG_XSDM_PRTY_STS);
				}
				break;
			case AEU_INPUTS_ATTN_BITS_XCM_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++, "XCM");
					_print_parity(bp, XCM_REG_XCM_PRTY_STS);
				}
				break;
			case AEU_INPUTS_ATTN_BITS_XSEMI_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++,
							  "XSEMI");
					_print_parity(bp,
						      XSEM_REG_XSEM_PRTY_STS_0);
					_print_parity(bp,
						      XSEM_REG_XSEM_PRTY_STS_1);
				}
				break;
			case AEU_INPUTS_ATTN_BITS_DOORBELLQ_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++,
							  "DOORBELLQ");
					_print_parity(bp,
						      DORQ_REG_DORQ_PRTY_STS);
				}
				break;
			case AEU_INPUTS_ATTN_BITS_NIG_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++, "NIG");
					if (CHIP_IS_E1x(bp)) {
						_print_parity(bp,
							NIG_REG_NIG_PRTY_STS);
					} else {
						_print_parity(bp,
							NIG_REG_NIG_PRTY_STS_0);
						_print_parity(bp,
							NIG_REG_NIG_PRTY_STS_1);
					}
				}
				break;
			case AEU_INPUTS_ATTN_BITS_VAUX_PCI_CORE_PARITY_ERROR:
				if (print)
					_print_next_block((*par_num)++,
							  "VAUX PCI CORE");
				*global = true;
				break;
			case AEU_INPUTS_ATTN_BITS_DEBUG_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++,
							  "DEBUG");
					_print_parity(bp, DBG_REG_DBG_PRTY_STS);
				}
				break;
			case AEU_INPUTS_ATTN_BITS_USDM_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++, "USDM");
					_print_parity(bp,
						      USDM_REG_USDM_PRTY_STS);
				}
				break;
			case AEU_INPUTS_ATTN_BITS_UCM_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++, "UCM");
					_print_parity(bp, UCM_REG_UCM_PRTY_STS);
				}
				break;
			case AEU_INPUTS_ATTN_BITS_USEMI_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++,
							  "USEMI");
					_print_parity(bp,
						      USEM_REG_USEM_PRTY_STS_0);
					_print_parity(bp,
						      USEM_REG_USEM_PRTY_STS_1);
				}
				break;
			case AEU_INPUTS_ATTN_BITS_UPB_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++, "UPB");
					_print_parity(bp, GRCBASE_UPB +
							  PB_REG_PB_PRTY_STS);
				}
				break;
			case AEU_INPUTS_ATTN_BITS_CSDM_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++, "CSDM");
					_print_parity(bp,
						      CSDM_REG_CSDM_PRTY_STS);
				}
				break;
			case AEU_INPUTS_ATTN_BITS_CCM_PARITY_ERROR:
				if (print) {
					_print_next_block((*par_num)++, "CCM");
					_print_parity(bp, CCM_REG_CCM_PRTY_STS);
				}
				break;
			}

			/* Clear the bit */
			sig &= ~cur_bit;
		}
	}

	return res;
}