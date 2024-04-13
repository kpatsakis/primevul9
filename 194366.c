tty_ldisc_lock_pair_timeout(struct tty_struct *tty, struct tty_struct *tty2,
			    unsigned long timeout)
{
	int ret;

	if (tty < tty2) {
		ret = __tty_ldisc_lock(tty, timeout);
		if (ret) {
			ret = __tty_ldisc_lock_nested(tty2, timeout);
			if (!ret)
				__tty_ldisc_unlock(tty);
		}
	} else {
		/* if this is possible, it has lots of implications */
		WARN_ON_ONCE(tty == tty2);
		if (tty2 && tty != tty2) {
			ret = __tty_ldisc_lock(tty2, timeout);
			if (ret) {
				ret = __tty_ldisc_lock_nested(tty, timeout);
				if (!ret)
					__tty_ldisc_unlock(tty2);
			}
		} else
			ret = __tty_ldisc_lock(tty, timeout);
	}

	if (!ret)
		return -EBUSY;

	set_bit(TTY_LDISC_HALTED, &tty->flags);
	if (tty2)
		set_bit(TTY_LDISC_HALTED, &tty2->flags);
	return 0;
}