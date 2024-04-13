static u32 iwl_trans_pcie_read_prph(struct iwl_trans *trans, u32 reg)
{
	u32 mask = iwl_trans_pcie_prph_msk(trans);

	iwl_trans_pcie_write32(trans, HBUS_TARG_PRPH_RADDR,
			       ((reg & mask) | (3 << 24)));
	return iwl_trans_pcie_read32(trans, HBUS_TARG_PRPH_RDAT);
}