static void mce_panic(const char *msg, struct mce *final, char *exp)
{
	int apei_err = 0;
	struct llist_node *pending;
	struct mce_evt_llist *l;

	if (!fake_panic) {
		/*
		 * Make sure only one CPU runs in machine check panic
		 */
		if (atomic_inc_return(&mce_panicked) > 1)
			wait_for_panic();
		barrier();

		bust_spinlocks(1);
		console_verbose();
	} else {
		/* Don't log too much for fake panic */
		if (atomic_inc_return(&mce_fake_panicked) > 1)
			return;
	}
	pending = mce_gen_pool_prepare_records();
	/* First print corrected ones that are still unlogged */
	llist_for_each_entry(l, pending, llnode) {
		struct mce *m = &l->mce;
		if (!(m->status & MCI_STATUS_UC)) {
			print_mce(m);
			if (!apei_err)
				apei_err = apei_write_mce(m);
		}
	}
	/* Now print uncorrected but with the final one last */
	llist_for_each_entry(l, pending, llnode) {
		struct mce *m = &l->mce;
		if (!(m->status & MCI_STATUS_UC))
			continue;
		if (!final || mce_cmp(m, final)) {
			print_mce(m);
			if (!apei_err)
				apei_err = apei_write_mce(m);
		}
	}
	if (final) {
		print_mce(final);
		if (!apei_err)
			apei_err = apei_write_mce(final);
	}
	if (cpu_missing)
		pr_emerg(HW_ERR "Some CPUs didn't answer in synchronization\n");
	if (exp)
		pr_emerg(HW_ERR "Machine check: %s\n", exp);
	if (!fake_panic) {
		if (panic_timeout == 0)
			panic_timeout = mca_cfg.panic_timeout;
		panic(msg);
	} else
		pr_emerg(HW_ERR "Fake kernel panic: %s\n", msg);
}