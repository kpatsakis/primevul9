static void i40e_service_timer(struct timer_list *t)
{
	struct i40e_pf *pf = from_timer(pf, t, service_timer);

	mod_timer(&pf->service_timer,
		  round_jiffies(jiffies + pf->service_timer_period));
	i40e_service_event_schedule(pf);
}