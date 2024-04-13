static void i40e_reset_and_rebuild(struct i40e_pf *pf, bool reinit,
				   bool lock_acquired)
{
	int ret;
	/* Now we wait for GRST to settle out.
	 * We don't have to delete the VEBs or VSIs from the hw switch
	 * because the reset will make them disappear.
	 */
	ret = i40e_reset(pf);
	if (!ret)
		i40e_rebuild(pf, reinit, lock_acquired);
}