void i40e_service_event_schedule(struct i40e_pf *pf)
{
	if ((!test_bit(__I40E_DOWN, pf->state) &&
	     !test_bit(__I40E_RESET_RECOVERY_PENDING, pf->state)) ||
	      test_bit(__I40E_RECOVERY_MODE, pf->state))
		queue_work(i40e_wq, &pf->service_task);
}