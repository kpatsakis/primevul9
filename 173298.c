void bnx2x_prep_dmae_with_comp(struct bnx2x *bp,
				      struct dmae_command *dmae,
				      u8 src_type, u8 dst_type)
{
	memset(dmae, 0, sizeof(struct dmae_command));

	/* set the opcode */
	dmae->opcode = bnx2x_dmae_opcode(bp, src_type, dst_type,
					 true, DMAE_COMP_PCI);

	/* fill in the completion parameters */
	dmae->comp_addr_lo = U64_LO(bnx2x_sp_mapping(bp, wb_comp));
	dmae->comp_addr_hi = U64_HI(bnx2x_sp_mapping(bp, wb_comp));
	dmae->comp_val = DMAE_COMP_VAL;
}