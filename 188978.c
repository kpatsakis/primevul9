static void i40e_handle_reset_warning(struct i40e_pf *pf, bool lock_acquired)
{
	i40e_prep_for_reset(pf, lock_acquired);
	i40e_reset_and_rebuild(pf, false, lock_acquired);
}