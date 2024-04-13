__tty_ldisc_lock_nested(struct tty_struct *tty, unsigned long timeout)
{
	return ldsem_down_write_nested(&tty->ldisc_sem,
				       LDISC_SEM_OTHER, timeout);
}