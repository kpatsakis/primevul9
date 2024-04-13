static int ql_wait_for_drvr_lock(struct ql3_adapter *qdev)
{
	int i = 0;

	do {
		if (ql_sem_lock(qdev,
				QL_DRVR_SEM_MASK,
				(QL_RESOURCE_BITS_BASE_CODE | (qdev->mac_index)
				 * 2) << 1)) {
			netdev_printk(KERN_DEBUG, qdev->ndev,
				      "driver lock acquired\n");
			return 1;
		}
		ssleep(1);
	} while (++i < 10);

	netdev_err(qdev->ndev, "Timed out waiting for driver lock...\n");
	return 0;
}