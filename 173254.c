u32 bnx2x_dmae_opcode_add_comp(u32 opcode, u8 comp_type)
{
	return opcode | ((comp_type << DMAE_COMMAND_C_DST_SHIFT) |
			   DMAE_CMD_C_ENABLE);
}