vcs_vc(struct inode *inode, int *viewed)
{
	unsigned int currcons = console(inode);

	WARN_CONSOLE_UNLOCKED();

	if (currcons == 0) {
		currcons = fg_console;
		if (viewed)
			*viewed = 1;
	} else {
		currcons--;
		if (viewed)
			*viewed = 0;
	}
	return vc_cons[currcons].d;
}