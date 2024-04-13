static int ql_port_start(struct ql3_adapter *qdev)
{
	if (ql_sem_spinlock(qdev, QL_PHY_GIO_SEM_MASK,
		(QL_RESOURCE_BITS_BASE_CODE | (qdev->mac_index) *
			 2) << 7)) {
		netdev_err(qdev->ndev, "Could not get hw lock for GIO\n");
		return -1;
	}

	if (ql_is_fiber(qdev)) {
		ql_petbi_init(qdev);
	} else {
		/* Copper port */
		ql_phy_init_ex(qdev);
	}

	ql_sem_unlock(qdev, QL_PHY_GIO_SEM_MASK);
	return 0;
}