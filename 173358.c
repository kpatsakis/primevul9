static bool bnx2x_check_blocks_with_parity4(struct bnx2x *bp, u32 sig,
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
				case AEU_INPUTS_ATTN_BITS_PGLUE_PARITY_ERROR:
					_print_next_block((*par_num)++,
							  "PGLUE_B");
					_print_parity(bp,
						      PGLUE_B_REG_PGLUE_B_PRTY_STS);
					break;
				case AEU_INPUTS_ATTN_BITS_ATC_PARITY_ERROR:
					_print_next_block((*par_num)++, "ATC");
					_print_parity(bp,
						      ATC_REG_ATC_PRTY_STS);
					break;
				}
			}
			/* Clear the bit */
			sig &= ~cur_bit;
		}
	}

	return res;
}