static void rds_iw_remove_one(struct ib_device *device)
{
	struct rds_iw_device *rds_iwdev;
	struct rds_iw_cm_id *i_cm_id, *next;

	rds_iwdev = ib_get_client_data(device, &rds_iw_client);
	if (!rds_iwdev)
		return;

	spin_lock_irq(&rds_iwdev->spinlock);
	list_for_each_entry_safe(i_cm_id, next, &rds_iwdev->cm_id_list, list) {
		list_del(&i_cm_id->list);
		kfree(i_cm_id);
	}
	spin_unlock_irq(&rds_iwdev->spinlock);

	rds_iw_destroy_conns(rds_iwdev);

	if (rds_iwdev->mr_pool)
		rds_iw_destroy_mr_pool(rds_iwdev->mr_pool);

	if (rds_iwdev->mr)
		ib_dereg_mr(rds_iwdev->mr);

	while (ib_dealloc_pd(rds_iwdev->pd)) {
		rdsdebug("Failed to dealloc pd %p\n", rds_iwdev->pd);
		msleep(1);
	}

	list_del(&rds_iwdev->list);
	kfree(rds_iwdev);
}