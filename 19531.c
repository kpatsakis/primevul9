static u32 iwl_trans_pcie_get_cmdlen(struct iwl_trans *trans, void *tfd)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	u32 cmdlen = 0;
	int i;

	for (i = 0; i < trans_pcie->max_tbs; i++)
		cmdlen += iwl_pcie_tfd_tb_get_len(trans, tfd, i);

	return cmdlen;
}