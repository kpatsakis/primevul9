int bnx2x_set_int_mode(struct bnx2x *bp)
{
	int rc = 0;

	if (IS_VF(bp) && int_mode != BNX2X_INT_MODE_MSIX) {
		BNX2X_ERR("VF not loaded since interrupt mode not msix\n");
		return -EINVAL;
	}

	switch (int_mode) {
	case BNX2X_INT_MODE_MSIX:
		/* attempt to enable msix */
		rc = bnx2x_enable_msix(bp);

		/* msix attained */
		if (!rc)
			return 0;

		/* vfs use only msix */
		if (rc && IS_VF(bp))
			return rc;

		/* failed to enable multiple MSI-X */
		BNX2X_DEV_INFO("Failed to enable multiple MSI-X (%d), set number of queues to %d\n",
			       bp->num_queues,
			       1 + bp->num_cnic_queues);

		/* falling through... */
	case BNX2X_INT_MODE_MSI:
		bnx2x_enable_msi(bp);

		/* falling through... */
	case BNX2X_INT_MODE_INTX:
		bp->num_ethernet_queues = 1;
		bp->num_queues = bp->num_ethernet_queues + bp->num_cnic_queues;
		BNX2X_DEV_INFO("set number of queues to 1\n");
		break;
	default:
		BNX2X_DEV_INFO("unknown value in int_mode module parameter\n");
		return -EINVAL;
	}
	return 0;
}