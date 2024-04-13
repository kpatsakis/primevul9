mounted (const char *spec0, const char *node0,
	 struct mntentchn *fstab_mc __attribute__((__unused__))) {

	struct mntentchn *mc, *mc0;
	const char *spec, *node;
	int ret = 0;

	/* Handle possible UUID= and LABEL= in spec */
	spec = spec_to_devname(spec0);
	if (!spec)
		return ret;

	node = canonicalize(node0);


	mc0 = mtab_head();
	for (mc = mc0->nxt; mc && mc != mc0; mc = mc->nxt)
		if (streq (spec, mc->m.mnt_fsname) &&
		    streq (node, mc->m.mnt_dir)) {
			ret = 1;
			break;
		}

	my_free(spec);
	my_free(node);

	return ret;
}