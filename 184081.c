	if(!Debug) {
		i = open(_PATH_TTY, O_RDWR|O_CLOEXEC);
		if (i >= 0) {
#			if !defined(__hpux)
				(void) ioctl(i, (int) TIOCNOTTY, NULL);
#			else
				/* TODO: we need to implement something for HP UX! -- rgerhards, 2008-03-04 */
				/* actually, HP UX should have setsid, so the code directly above should
				 * trigger. So the actual question is why it doesn't do that...
				 */
#			endif
			close(i);
		}
	}