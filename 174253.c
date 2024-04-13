static __init int selinux_init(void)
{
	pr_info("SELinux:  Initializing.\n");

	memset(&selinux_state, 0, sizeof(selinux_state));
	enforcing_set(&selinux_state, selinux_enforcing_boot);
	selinux_state.checkreqprot = selinux_checkreqprot_boot;
	selinux_ss_init(&selinux_state.ss);
	selinux_avc_init(&selinux_state.avc);
	mutex_init(&selinux_state.status_lock);

	/* Set the security state for the initial task. */
	cred_init_security();

	default_noexec = !(VM_DATA_DEFAULT_FLAGS & VM_EXEC);

	avc_init();

	avtab_cache_init();

	ebitmap_cache_init();

	hashtab_cache_init();

	security_add_hooks(selinux_hooks, ARRAY_SIZE(selinux_hooks), "selinux");

	if (avc_add_callback(selinux_netcache_avc_callback, AVC_CALLBACK_RESET))
		panic("SELinux: Unable to register AVC netcache callback\n");

	if (avc_add_callback(selinux_lsm_notifier_avc_callback, AVC_CALLBACK_RESET))
		panic("SELinux: Unable to register AVC LSM notifier callback\n");

	if (selinux_enforcing_boot)
		pr_debug("SELinux:  Starting in enforcing mode\n");
	else
		pr_debug("SELinux:  Starting in permissive mode\n");

	fs_validate_description("selinux", selinux_fs_parameters);

	return 0;
}