static void rds_iw_add_one(struct ib_device *device)
{
	struct rds_iw_device *rds_iwdev;
	struct ib_device_attr *dev_attr;

	/* Only handle iwarp devices */
	if (device->node_type != RDMA_NODE_RNIC)
		return;

	dev_attr = kmalloc(sizeof *dev_attr, GFP_KERNEL);
	if (!dev_attr)
		return;

	if (ib_query_device(device, dev_attr)) {
		rdsdebug("Query device failed for %s\n", device->name);
		goto free_attr;
	}

	rds_iwdev = kmalloc(sizeof *rds_iwdev, GFP_KERNEL);
	if (!rds_iwdev)
		goto free_attr;

	spin_lock_init(&rds_iwdev->spinlock);

	rds_iwdev->dma_local_lkey = !!(dev_attr->device_cap_flags & IB_DEVICE_LOCAL_DMA_LKEY);
	rds_iwdev->max_wrs = dev_attr->max_qp_wr;
	rds_iwdev->max_sge = min(dev_attr->max_sge, RDS_IW_MAX_SGE);

	rds_iwdev->dev = device;
	rds_iwdev->pd = ib_alloc_pd(device);
	if (IS_ERR(rds_iwdev->pd))
		goto free_dev;

	if (!rds_iwdev->dma_local_lkey) {
		rds_iwdev->mr = ib_get_dma_mr(rds_iwdev->pd,
					IB_ACCESS_REMOTE_READ |
					IB_ACCESS_REMOTE_WRITE |
					IB_ACCESS_LOCAL_WRITE);
		if (IS_ERR(rds_iwdev->mr))
			goto err_pd;
	} else
		rds_iwdev->mr = NULL;

	rds_iwdev->mr_pool = rds_iw_create_mr_pool(rds_iwdev);
	if (IS_ERR(rds_iwdev->mr_pool)) {
		rds_iwdev->mr_pool = NULL;
		goto err_mr;
	}

	INIT_LIST_HEAD(&rds_iwdev->cm_id_list);
	INIT_LIST_HEAD(&rds_iwdev->conn_list);
	list_add_tail(&rds_iwdev->list, &rds_iw_devices);

	ib_set_client_data(device, &rds_iw_client, rds_iwdev);

	goto free_attr;

err_mr:
	if (rds_iwdev->mr)
		ib_dereg_mr(rds_iwdev->mr);
err_pd:
	ib_dealloc_pd(rds_iwdev->pd);
free_dev:
	kfree(rds_iwdev);
free_attr:
	kfree(dev_attr);
}