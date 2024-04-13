static u32 iwl_trans_pcie_prph_msk(struct iwl_trans *trans)
{
	if (trans->trans_cfg->device_family >= IWL_DEVICE_FAMILY_22560)
		return 0x00FFFFFF;
	else
		return 0x000FFFFF;
}