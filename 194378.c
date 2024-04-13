static inline void __tty_ldisc_unlock(struct tty_struct *tty)
{
	ldsem_up_write(&tty->ldisc_sem);
}