static void bnx2x_dp_dmae(struct bnx2x *bp,
			  struct dmae_command *dmae, int msglvl)
{
	u32 src_type = dmae->opcode & DMAE_COMMAND_SRC;
	int i;

	switch (dmae->opcode & DMAE_COMMAND_DST) {
	case DMAE_CMD_DST_PCI:
		if (src_type == DMAE_CMD_SRC_PCI)
			DP(msglvl, "DMAE: opcode 0x%08x\n"
			   "src [%x:%08x], len [%d*4], dst [%x:%08x]\n"
			   "comp_addr [%x:%08x], comp_val 0x%08x\n",
			   dmae->opcode, dmae->src_addr_hi, dmae->src_addr_lo,
			   dmae->len, dmae->dst_addr_hi, dmae->dst_addr_lo,
			   dmae->comp_addr_hi, dmae->comp_addr_lo,
			   dmae->comp_val);
		else
			DP(msglvl, "DMAE: opcode 0x%08x\n"
			   "src [%08x], len [%d*4], dst [%x:%08x]\n"
			   "comp_addr [%x:%08x], comp_val 0x%08x\n",
			   dmae->opcode, dmae->src_addr_lo >> 2,
			   dmae->len, dmae->dst_addr_hi, dmae->dst_addr_lo,
			   dmae->comp_addr_hi, dmae->comp_addr_lo,
			   dmae->comp_val);
		break;
	case DMAE_CMD_DST_GRC:
		if (src_type == DMAE_CMD_SRC_PCI)
			DP(msglvl, "DMAE: opcode 0x%08x\n"
			   "src [%x:%08x], len [%d*4], dst_addr [%08x]\n"
			   "comp_addr [%x:%08x], comp_val 0x%08x\n",
			   dmae->opcode, dmae->src_addr_hi, dmae->src_addr_lo,
			   dmae->len, dmae->dst_addr_lo >> 2,
			   dmae->comp_addr_hi, dmae->comp_addr_lo,
			   dmae->comp_val);
		else
			DP(msglvl, "DMAE: opcode 0x%08x\n"
			   "src [%08x], len [%d*4], dst [%08x]\n"
			   "comp_addr [%x:%08x], comp_val 0x%08x\n",
			   dmae->opcode, dmae->src_addr_lo >> 2,
			   dmae->len, dmae->dst_addr_lo >> 2,
			   dmae->comp_addr_hi, dmae->comp_addr_lo,
			   dmae->comp_val);
		break;
	default:
		if (src_type == DMAE_CMD_SRC_PCI)
			DP(msglvl, "DMAE: opcode 0x%08x\n"
			   "src_addr [%x:%08x]  len [%d * 4]  dst_addr [none]\n"
			   "comp_addr [%x:%08x]  comp_val 0x%08x\n",
			   dmae->opcode, dmae->src_addr_hi, dmae->src_addr_lo,
			   dmae->len, dmae->comp_addr_hi, dmae->comp_addr_lo,
			   dmae->comp_val);
		else
			DP(msglvl, "DMAE: opcode 0x%08x\n"
			   "src_addr [%08x]  len [%d * 4]  dst_addr [none]\n"
			   "comp_addr [%x:%08x]  comp_val 0x%08x\n",
			   dmae->opcode, dmae->src_addr_lo >> 2,
			   dmae->len, dmae->comp_addr_hi, dmae->comp_addr_lo,
			   dmae->comp_val);
		break;
	}

	for (i = 0; i < (sizeof(struct dmae_command)/4); i++)
		DP(msglvl, "DMAE RAW [%02d]: 0x%08x\n",
		   i, *(((u32 *)dmae) + i));
}