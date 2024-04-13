u32 bnx2x_dmae_opcode_clr_src_reset(u32 opcode)
{
	return opcode & ~DMAE_CMD_SRC_RESET;
}