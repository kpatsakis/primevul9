void exit_itimers(struct signal_struct *sig)
{
	struct k_itimer *tmr;

	while (!list_empty(&sig->posix_timers)) {
		tmr = list_entry(sig->posix_timers.next, struct k_itimer, list);
		itimer_delete(tmr);
	}
}