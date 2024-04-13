static void i40e_reenable_fdir_atr(struct i40e_pf *pf)
{
	if (test_and_clear_bit(__I40E_FD_ATR_AUTO_DISABLED, pf->state)) {
		/* ATR uses the same filtering logic as SB rules. It only
		 * functions properly if the input set mask is at the default
		 * settings. It is safe to restore the default input set
		 * because there are no active TCPv4 filter rules.
		 */
		i40e_write_fd_input_set(pf, I40E_FILTER_PCTYPE_NONF_IPV4_TCP,
					I40E_L3_SRC_MASK | I40E_L3_DST_MASK |
					I40E_L4_SRC_MASK | I40E_L4_DST_MASK);

		if ((pf->flags & I40E_FLAG_FD_ATR_ENABLED) &&
		    (I40E_DEBUG_FD & pf->hw.debug_mask))
			dev_info(&pf->pdev->dev, "ATR is being enabled since we have space in the table and there are no conflicting ntuple rules\n");
	}
}