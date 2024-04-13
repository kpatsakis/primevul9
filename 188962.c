static int __maybe_unused i40e_resume(struct device *dev)
{
	struct i40e_pf *pf = dev_get_drvdata(dev);
	int err;

	/* If we're not suspended, then there is nothing to do */
	if (!test_bit(__I40E_SUSPENDED, pf->state))
		return 0;

	/* We need to hold the RTNL lock prior to restoring interrupt schemes,
	 * since we're going to be restoring queues
	 */
	rtnl_lock();

	/* We cleared the interrupt scheme when we suspended, so we need to
	 * restore it now to resume device functionality.
	 */
	err = i40e_restore_interrupt_scheme(pf);
	if (err) {
		dev_err(dev, "Cannot restore interrupt scheme: %d\n",
			err);
	}

	clear_bit(__I40E_DOWN, pf->state);
	i40e_reset_and_rebuild(pf, false, true);

	rtnl_unlock();

	/* Clear suspended state last after everything is recovered */
	clear_bit(__I40E_SUSPENDED, pf->state);

	/* Restart the service task */
	mod_timer(&pf->service_timer,
		  round_jiffies(jiffies + pf->service_timer_period));

	return 0;
}