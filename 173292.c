static bool bnx2x_check_blocks_with_parity0(struct bnx2x *bp, u32 sig,
					    int *par_num, bool print)
{
	u32 cur_bit;
	bool res;
	int i;

	res = false;

	for (i = 0; sig; i++) {
		cur_bit = (0x1UL << i);
		if (sig & cur_bit) {
			res |= true; /* Each bit is real error! */

			if (print) {
				switch (cur_bit) {
				case AEU_INPUTS_ATTN_BITS_BRB_PARITY_ERROR:
					_print_next_block((*par_num)++, "BRB");
					_print_parity(bp,
						      BRB1_REG_BRB1_PRTY_STS);
					break;
				case AEU_INPUTS_ATTN_BITS_PARSER_PARITY_ERROR:
					_print_next_block((*par_num)++,
							  "PARSER");
					_print_parity(bp, PRS_REG_PRS_PRTY_STS);
					break;
				case AEU_INPUTS_ATTN_BITS_TSDM_PARITY_ERROR:
					_print_next_block((*par_num)++, "TSDM");
					_print_parity(bp,
						      TSDM_REG_TSDM_PRTY_STS);
					break;
				case AEU_INPUTS_ATTN_BITS_SEARCHER_PARITY_ERROR:
					_print_next_block((*par_num)++,
							  "SEARCHER");
					_print_parity(bp, SRC_REG_SRC_PRTY_STS);
					break;
				case AEU_INPUTS_ATTN_BITS_TCM_PARITY_ERROR:
					_print_next_block((*par_num)++, "TCM");
					_print_parity(bp, TCM_REG_TCM_PRTY_STS);
					break;
				case AEU_INPUTS_ATTN_BITS_TSEMI_PARITY_ERROR:
					_print_next_block((*par_num)++,
							  "TSEMI");
					_print_parity(bp,
						      TSEM_REG_TSEM_PRTY_STS_0);
					_print_parity(bp,
						      TSEM_REG_TSEM_PRTY_STS_1);
					break;
				case AEU_INPUTS_ATTN_BITS_PBCLIENT_PARITY_ERROR:
					_print_next_block((*par_num)++, "XPB");
					_print_parity(bp, GRCBASE_XPB +
							  PB_REG_PB_PRTY_STS);
					break;
				}
			}

			/* Clear the bit */
			sig &= ~cur_bit;
		}
	}

	return res;
}