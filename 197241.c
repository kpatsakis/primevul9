getfs(const char *spec, const char *uuid, const char *label)
{
	struct mntentchn *mc = NULL;
	const char *devname = NULL;

	if (!spec && !uuid && !label)
		return NULL;

	/*
	 * A) 99% of all cases, the spec on cmdline matches
	 *    with spec in fstab
	 */
	if (uuid)
		mc = getfs_by_uuid(uuid);
	else if (label)
		mc = getfs_by_label(label);
	else {
		mc = getfs_by_dir(spec);

		if (!mc)
			mc = getfs_by_spec(spec);
	}
	if (mc)
		return mc;

	/*
	 * B) UUID or LABEL on cmdline, but devname in fstab
	 */
	if (uuid)
		devname = fsprobe_get_devname_by_uuid(uuid);
	else if (label)
		devname = fsprobe_get_devname_by_label(label);
	else
		devname = spec_to_devname(spec);

	if (devname)
		mc = getfs_by_devname(devname);

	/*
	 * C) mixed
	 */
	if (!mc && devname) {
		const char *id = NULL;

		if (!label && (!spec || strncmp(spec, "LABEL=", 6))) {
			id = fsprobe_get_label_by_devname(devname);
			if (id)
				mc = getfs_by_label(id);
		}
		if (!mc && !uuid && (!spec || strncmp(spec, "UUID=", 5))) {
			id = fsprobe_get_uuid_by_devname(devname);
			if (id)
				mc = getfs_by_uuid(id);
		}
		my_free(id);

		if (mc) {
			/* use real device name to avoid repetitional
			 * conversion from LABEL/UUID to devname
			 */
			my_free(mc->m.mnt_fsname);
			mc->m.mnt_fsname = xstrdup(devname);
		}
	}

	/*
	 * D) remount -- try /etc/mtab
	 *    Earlier mtab was tried first, but this would sometimes try the
	 *    wrong mount in case mtab had the root device entry wrong.  Try
	 *    the last occurrence first, since that is the visible mount.
	 */
	if (!mc && (devname || spec))
		mc = getmntfilebackward (devname ? devname : spec, NULL);

	my_free(devname);
	return mc;
}