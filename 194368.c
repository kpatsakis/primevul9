void tty_ldisc_deref(struct tty_ldisc *ld)
{
	ldsem_up_read(&ld->tty->ldisc_sem);
}