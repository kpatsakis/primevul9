bool machine_check_poll(enum mcp_flags flags, mce_banks_t *b)
{
	bool error_seen = false;
	struct mce m;
	int i;

	this_cpu_inc(mce_poll_count);

	mce_gather_info(&m, NULL);

	if (flags & MCP_TIMESTAMP)
		m.tsc = rdtsc();

	for (i = 0; i < mca_cfg.banks; i++) {
		if (!mce_banks[i].ctl || !test_bit(i, *b))
			continue;

		m.misc = 0;
		m.addr = 0;
		m.bank = i;

		barrier();
		m.status = mce_rdmsrl(msr_ops.status(i));
		if (!(m.status & MCI_STATUS_VAL))
			continue;

		/*
		 * Uncorrected or signalled events are handled by the exception
		 * handler when it is enabled, so don't process those here.
		 *
		 * TBD do the same check for MCI_STATUS_EN here?
		 */
		if (!(flags & MCP_UC) &&
		    (m.status & (mca_cfg.ser ? MCI_STATUS_S : MCI_STATUS_UC)))
			continue;

		error_seen = true;

		mce_read_aux(&m, i);

		m.severity = mce_severity(&m, mca_cfg.tolerant, NULL, false);

		/*
		 * Don't get the IP here because it's unlikely to
		 * have anything to do with the actual error location.
		 */
		if (!(flags & MCP_DONTLOG) && !mca_cfg.dont_log_ce)
			mce_log(&m);
		else if (mce_usable_address(&m)) {
			/*
			 * Although we skipped logging this, we still want
			 * to take action. Add to the pool so the registered
			 * notifiers will see it.
			 */
			if (!mce_gen_pool_add(&m))
				mce_schedule_work();
		}

		/*
		 * Clear state for this bank.
		 */
		mce_wrmsrl(msr_ops.status(i), 0);
	}

	/*
	 * Don't clear MCG_STATUS here because it's only defined for
	 * exceptions.
	 */

	sync_core();

	return error_seen;
}