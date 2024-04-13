static void i40e_reset_subtask(struct i40e_pf *pf)
{
	u32 reset_flags = 0;

	if (test_bit(__I40E_REINIT_REQUESTED, pf->state)) {
		reset_flags |= BIT(__I40E_REINIT_REQUESTED);
		clear_bit(__I40E_REINIT_REQUESTED, pf->state);
	}
	if (test_bit(__I40E_PF_RESET_REQUESTED, pf->state)) {
		reset_flags |= BIT(__I40E_PF_RESET_REQUESTED);
		clear_bit(__I40E_PF_RESET_REQUESTED, pf->state);
	}
	if (test_bit(__I40E_CORE_RESET_REQUESTED, pf->state)) {
		reset_flags |= BIT(__I40E_CORE_RESET_REQUESTED);
		clear_bit(__I40E_CORE_RESET_REQUESTED, pf->state);
	}
	if (test_bit(__I40E_GLOBAL_RESET_REQUESTED, pf->state)) {
		reset_flags |= BIT(__I40E_GLOBAL_RESET_REQUESTED);
		clear_bit(__I40E_GLOBAL_RESET_REQUESTED, pf->state);
	}
	if (test_bit(__I40E_DOWN_REQUESTED, pf->state)) {
		reset_flags |= BIT(__I40E_DOWN_REQUESTED);
		clear_bit(__I40E_DOWN_REQUESTED, pf->state);
	}

	/* If there's a recovery already waiting, it takes
	 * precedence before starting a new reset sequence.
	 */
	if (test_bit(__I40E_RESET_INTR_RECEIVED, pf->state)) {
		i40e_prep_for_reset(pf, false);
		i40e_reset(pf);
		i40e_rebuild(pf, false, false);
	}

	/* If we're already down or resetting, just bail */
	if (reset_flags &&
	    !test_bit(__I40E_DOWN, pf->state) &&
	    !test_bit(__I40E_CONFIG_BUSY, pf->state)) {
		i40e_do_reset(pf, reset_flags, false);
	}
}