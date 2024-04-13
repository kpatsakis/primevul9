
static u32 ql_get_speed(struct ql3_adapter *qdev)
{
	u32 status;
	unsigned long hw_flags;
	spin_lock_irqsave(&qdev->hw_lock, hw_flags);
	if (ql_sem_spinlock(qdev, QL_PHY_GIO_SEM_MASK,
			    (QL_RESOURCE_BITS_BASE_CODE |
			     (qdev->mac_index) * 2) << 7)) {
		spin_unlock_irqrestore(&qdev->hw_lock, hw_flags);
		return 0;
	}
	status = ql_get_link_speed(qdev);
	ql_sem_unlock(qdev, QL_PHY_GIO_SEM_MASK);
	spin_unlock_irqrestore(&qdev->hw_lock, hw_flags);
	return status;