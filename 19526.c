static void iwl_trans_pcie_set_pmi(struct iwl_trans *trans, bool state)
{
	if (state)
		set_bit(STATUS_TPOWER_PMI, &trans->status);
	else
		clear_bit(STATUS_TPOWER_PMI, &trans->status);
}