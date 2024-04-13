static void i40e_service_task(struct work_struct *work)
{
	struct i40e_pf *pf = container_of(work,
					  struct i40e_pf,
					  service_task);
	unsigned long start_time = jiffies;

	/* don't bother with service tasks if a reset is in progress */
	if (test_bit(__I40E_RESET_RECOVERY_PENDING, pf->state) ||
	    test_bit(__I40E_SUSPENDED, pf->state))
		return;

	if (test_and_set_bit(__I40E_SERVICE_SCHED, pf->state))
		return;

	if (!test_bit(__I40E_RECOVERY_MODE, pf->state)) {
		i40e_detect_recover_hung(pf->vsi[pf->lan_vsi]);
		i40e_sync_filters_subtask(pf);
		i40e_reset_subtask(pf);
		i40e_handle_mdd_event(pf);
		i40e_vc_process_vflr_event(pf);
		i40e_watchdog_subtask(pf);
		i40e_fdir_reinit_subtask(pf);
		if (test_and_clear_bit(__I40E_CLIENT_RESET, pf->state)) {
			/* Client subtask will reopen next time through. */
			i40e_notify_client_of_netdev_close(pf->vsi[pf->lan_vsi],
							   true);
		} else {
			i40e_client_subtask(pf);
			if (test_and_clear_bit(__I40E_CLIENT_L2_CHANGE,
					       pf->state))
				i40e_notify_client_of_l2_param_changes(
								pf->vsi[pf->lan_vsi]);
		}
		i40e_sync_filters_subtask(pf);
		i40e_sync_udp_filters_subtask(pf);
	} else {
		i40e_reset_subtask(pf);
	}

	i40e_clean_adminq_subtask(pf);

	/* flush memory to make sure state is correct before next watchdog */
	smp_mb__before_atomic();
	clear_bit(__I40E_SERVICE_SCHED, pf->state);

	/* If the tasks have taken longer than one timer cycle or there
	 * is more work to be done, reschedule the service task now
	 * rather than wait for the timer to tick again.
	 */
	if (time_after(jiffies, (start_time + pf->service_timer_period)) ||
	    test_bit(__I40E_ADMINQ_EVENT_PENDING, pf->state)		 ||
	    test_bit(__I40E_MDD_EVENT_PENDING, pf->state)		 ||
	    test_bit(__I40E_VFLR_EVENT_PENDING, pf->state))
		i40e_service_event_schedule(pf);
}