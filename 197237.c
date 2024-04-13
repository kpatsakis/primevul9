print_one (const struct my_mntent *me) {

	char *fsname = NULL;

	if (mount_quiet)
		return;

	/* users assume backing file name rather than /dev/loopN in
	 * mount(8) output if the device has been initialized by mount(8).
	 */
	if (strncmp(me->mnt_fsname, "/dev/loop", 9) == 0 &&
	    loopdev_is_autoclear(me->mnt_fsname))
		fsname = loopdev_get_backing_file(me->mnt_fsname);

	if (!fsname)
		fsname = (char *) me->mnt_fsname;

	printf ("%s on %s", fsname, me->mnt_dir);
	if (me->mnt_type != NULL && *(me->mnt_type) != '\0')
		printf (" type %s", me->mnt_type);
	if (me->mnt_opts != NULL)
		printf (" (%s)", me->mnt_opts);
	if (list_with_volumelabel && is_pseudo_fs(me->mnt_type) == 0) {
		const char *devname = spec_to_devname(me->mnt_fsname);

		if (devname) {
			const char *label;

			label = fsprobe_get_label_by_devname(devname);
			my_free(devname);

			if (label) {
				printf (" [%s]", label);
				my_free(label);
			}
		}
	}
	printf ("\n");
}