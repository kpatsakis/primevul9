static __init int mcheck_init_device(void)
{
	int err;

	if (!mce_available(&boot_cpu_data)) {
		err = -EIO;
		goto err_out;
	}

	if (!zalloc_cpumask_var(&mce_device_initialized, GFP_KERNEL)) {
		err = -ENOMEM;
		goto err_out;
	}

	mce_init_banks();

	err = subsys_system_register(&mce_subsys, NULL);
	if (err)
		goto err_out_mem;

	err = cpuhp_setup_state(CPUHP_X86_MCE_DEAD, "x86/mce:dead", NULL,
				mce_cpu_dead);
	if (err)
		goto err_out_mem;

	err = cpuhp_setup_state(CPUHP_AP_ONLINE_DYN, "x86/mce:online",
				mce_cpu_online, mce_cpu_pre_down);
	if (err < 0)
		goto err_out_online;

	register_syscore_ops(&mce_syscore_ops);

	return 0;

err_out_online:
	cpuhp_remove_state(CPUHP_X86_MCE_DEAD);

err_out_mem:
	free_cpumask_var(mce_device_initialized);

err_out:
	pr_err("Unable to init MCE device (rc: %d)\n", err);

	return err;
}