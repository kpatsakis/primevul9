void timers_update_nohz(void)
{
	schedule_work(&timer_update_work);
}