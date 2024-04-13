
static void ql3xxx_tx_timeout(struct net_device *ndev)
{
	struct ql3_adapter *qdev = netdev_priv(ndev);

	netdev_err(ndev, "Resetting...\n");
	/*
	 * Stop the queues, we've got a problem.
	 */
	netif_stop_queue(ndev);

	/*
	 * Wake up the worker to process this event.
	 */
	queue_delayed_work(qdev->workqueue, &qdev->tx_timeout_work, 0);