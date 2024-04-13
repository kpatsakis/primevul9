is_mounted_same_loopfile(const char *node0, const char *loopfile, unsigned long long offset)
{
	struct mntentchn *mnt = NULL;
	char *node;
	int res = 0;

	node = canonicalize(node0);

	/* Search for mountpoint node in mtab,
	 * procceed if any of these has the loop option set or
	 * the device is a loop device
	 */
	mnt = getmntdirbackward(node, mnt);
	if (!mnt) {
		free(node);
		return 0;
	}
	for(; mnt && res == 0; mnt = getmntdirbackward(node, mnt)) {
		char *p;

		if (strncmp(mnt->m.mnt_fsname, "/dev/loop", 9) == 0)
			res = loopdev_is_used((char *) mnt->m.mnt_fsname,
					loopfile, offset, LOOPDEV_FL_OFFSET);

		else if (mnt->m.mnt_opts &&
			 (p = strstr(mnt->m.mnt_opts, "loop=")))
		{
			char *dev = xstrdup(p+5);
			if ((p = strchr(dev, ',')))
				*p = '\0';
			res =  loopdev_is_used(dev,
					loopfile, offset, LOOPDEV_FL_OFFSET);
			free(dev);
		}
	}

	free(node);
	return res;
}