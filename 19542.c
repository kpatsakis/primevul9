static void iwl_trans_pcie_write_prph(struct iwl_trans *trans, u32 addr,
				      u32 val)
{
	u32 mask = iwl_trans_pcie_prph_msk(trans);

	iwl_trans_pcie_write32(trans, HBUS_TARG_PRPH_WADDR,
			       ((addr & mask) | (3 << 24)));
	iwl_trans_pcie_write32(trans, HBUS_TARG_PRPH_WDAT, val);
}