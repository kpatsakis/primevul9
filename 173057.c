static bool bnx2x_check_blocks_with_parity3(struct bnx2x *bp, u32 sig,
					    int *par_num, bool *global,
					    bool print)
{
	bool res = false;
	u32 cur_bit;
	int i;

	for (i = 0; sig; i++) {
		cur_bit = (0x1UL << i);
		if (sig & cur_bit) {
			switch (cur_bit) {
			case AEU_INPUTS_ATTN_BITS_MCP_LATCHED_ROM_PARITY:
				if (print)
					_print_next_block((*par_num)++,
							  "MCP ROM");
				*global = true;
				res = true;
				break;
			case AEU_INPUTS_ATTN_BITS_MCP_LATCHED_UMP_RX_PARITY:
				if (print)
					_print_next_block((*par_num)++,
							  "MCP UMP RX");
				*global = true;
				res = true;
				break;
			case AEU_INPUTS_ATTN_BITS_MCP_LATCHED_UMP_TX_PARITY:
				if (print)
					_print_next_block((*par_num)++,
							  "MCP UMP TX");
				*global = true;
				res = true;
				break;
			case AEU_INPUTS_ATTN_BITS_MCP_LATCHED_SCPAD_PARITY:
				(*par_num)++;
				/* clear latched SCPAD PATIRY from MCP */
				REG_WR(bp, MISC_REG_AEU_CLR_LATCH_SIGNAL,
				       1UL << 10);
				break;
			}

			/* Clear the bit */
			sig &= ~cur_bit;
		}
	}

	return res;
}