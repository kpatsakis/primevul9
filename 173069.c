static bool bnx2x_check_blocks_with_parity2(struct bnx2x *bp, u32 sig,
					    int *par_num, bool print)
{
	u32 cur_bit;
	bool res;
	int i;

	res = false;

	for (i = 0; sig; i++) {
		cur_bit = (0x1UL << i);
		if (sig & cur_bit) {
			res = true; /* Each bit is real error! */
			if (print) {
				switch (cur_bit) {
				case AEU_INPUTS_ATTN_BITS_CSEMI_PARITY_ERROR:
					_print_next_block((*par_num)++,
							  "CSEMI");
					_print_parity(bp,
						      CSEM_REG_CSEM_PRTY_STS_0);
					_print_parity(bp,
						      CSEM_REG_CSEM_PRTY_STS_1);
					break;
				case AEU_INPUTS_ATTN_BITS_PXP_PARITY_ERROR:
					_print_next_block((*par_num)++, "PXP");
					_print_parity(bp, PXP_REG_PXP_PRTY_STS);
					_print_parity(bp,
						      PXP2_REG_PXP2_PRTY_STS_0);
					_print_parity(bp,
						      PXP2_REG_PXP2_PRTY_STS_1);
					break;
				case AEU_IN_ATTN_BITS_PXPPCICLOCKCLIENT_PARITY_ERROR:
					_print_next_block((*par_num)++,
							  "PXPPCICLOCKCLIENT");
					break;
				case AEU_INPUTS_ATTN_BITS_CFC_PARITY_ERROR:
					_print_next_block((*par_num)++, "CFC");
					_print_parity(bp,
						      CFC_REG_CFC_PRTY_STS);
					break;
				case AEU_INPUTS_ATTN_BITS_CDU_PARITY_ERROR:
					_print_next_block((*par_num)++, "CDU");
					_print_parity(bp, CDU_REG_CDU_PRTY_STS);
					break;
				case AEU_INPUTS_ATTN_BITS_DMAE_PARITY_ERROR:
					_print_next_block((*par_num)++, "DMAE");
					_print_parity(bp,
						      DMAE_REG_DMAE_PRTY_STS);
					break;
				case AEU_INPUTS_ATTN_BITS_IGU_PARITY_ERROR:
					_print_next_block((*par_num)++, "IGU");
					if (CHIP_IS_E1x(bp))
						_print_parity(bp,
							HC_REG_HC_PRTY_STS);
					else
						_print_parity(bp,
							IGU_REG_IGU_PRTY_STS);
					break;
				case AEU_INPUTS_ATTN_BITS_MISC_PARITY_ERROR:
					_print_next_block((*par_num)++, "MISC");
					_print_parity(bp,
						      MISC_REG_MISC_PRTY_STS);
					break;
				}
			}

			/* Clear the bit */
			sig &= ~cur_bit;
		}
	}

	return res;
}