u32 bnx2x_dmae_opcode(struct bnx2x *bp, u8 src_type, u8 dst_type,
			     bool with_comp, u8 comp_type)
{
	u32 opcode = 0;

	opcode |= ((src_type << DMAE_COMMAND_SRC_SHIFT) |
		   (dst_type << DMAE_COMMAND_DST_SHIFT));

	opcode |= (DMAE_CMD_SRC_RESET | DMAE_CMD_DST_RESET);

	opcode |= (BP_PORT(bp) ? DMAE_CMD_PORT_1 : DMAE_CMD_PORT_0);
	opcode |= ((BP_VN(bp) << DMAE_CMD_E1HVN_SHIFT) |
		   (BP_VN(bp) << DMAE_COMMAND_DST_VN_SHIFT));
	opcode |= (DMAE_COM_SET_ERR << DMAE_COMMAND_ERR_POLICY_SHIFT);

#ifdef __BIG_ENDIAN
	opcode |= DMAE_CMD_ENDIANITY_B_DW_SWAP;
#else
	opcode |= DMAE_CMD_ENDIANITY_DW_SWAP;
#endif
	if (with_comp)
		opcode = bnx2x_dmae_opcode_add_comp(opcode, comp_type);
	return opcode;
}