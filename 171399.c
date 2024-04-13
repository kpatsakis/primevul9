static void ql_link_state_machine_work(struct work_struct *work)
{
	struct ql3_adapter *qdev =
		container_of(work, struct ql3_adapter, link_state_work.work);

	u32 curr_link_state;
	unsigned long hw_flags;

	spin_lock_irqsave(&qdev->hw_lock, hw_flags);

	curr_link_state = ql_get_link_state(qdev);

	if (test_bit(QL_RESET_ACTIVE, &qdev->flags)) {
		netif_info(qdev, link, qdev->ndev,
			   "Reset in progress, skip processing link state\n");

		spin_unlock_irqrestore(&qdev->hw_lock, hw_flags);

		/* Restart timer on 2 second interval. */
		mod_timer(&qdev->adapter_timer, jiffies + HZ * 1);

		return;
	}

	switch (qdev->port_link_state) {
	default:
		if (test_bit(QL_LINK_MASTER, &qdev->flags))
			ql_port_start(qdev);
		qdev->port_link_state = LS_DOWN;
		/* Fall Through */

	case LS_DOWN:
		if (curr_link_state == LS_UP) {
			netif_info(qdev, link, qdev->ndev, "Link is up\n");
			if (ql_is_auto_neg_complete(qdev))
				ql_finish_auto_neg(qdev);

			if (qdev->port_link_state == LS_UP)
				ql_link_down_detect_clear(qdev);

			qdev->port_link_state = LS_UP;
		}
		break;

	case LS_UP:
		/*
		 * See if the link is currently down or went down and came
		 * back up
		 */
		if (curr_link_state == LS_DOWN) {
			netif_info(qdev, link, qdev->ndev, "Link is down\n");
			qdev->port_link_state = LS_DOWN;
		}
		if (ql_link_down_detect(qdev))
			qdev->port_link_state = LS_DOWN;
		break;
	}
	spin_unlock_irqrestore(&qdev->hw_lock, hw_flags);

	/* Restart timer on 2 second interval. */
	mod_timer(&qdev->adapter_timer, jiffies + HZ * 1);
}