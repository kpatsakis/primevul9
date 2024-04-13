static void i40e_fdir_reinit_subtask(struct i40e_pf *pf)
{

	/* if interface is down do nothing */
	if (test_bit(__I40E_DOWN, pf->state))
		return;

	if (test_bit(__I40E_FD_FLUSH_REQUESTED, pf->state))
		i40e_fdir_flush_and_replay(pf);

	i40e_fdir_check_and_reenable(pf);

}