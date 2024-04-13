
static int ql_alloc_buffer_queues(struct ql3_adapter *qdev)
{
	/* Create Large Buffer Queue */
	qdev->lrg_buf_q_size =
		qdev->num_lbufq_entries * sizeof(struct lrg_buf_q_entry);
	if (qdev->lrg_buf_q_size < PAGE_SIZE)
		qdev->lrg_buf_q_alloc_size = PAGE_SIZE;
	else
		qdev->lrg_buf_q_alloc_size = qdev->lrg_buf_q_size * 2;

	qdev->lrg_buf = kmalloc_array(qdev->num_large_buffers,
				      sizeof(struct ql_rcv_buf_cb),
				      GFP_KERNEL);
	if (qdev->lrg_buf == NULL)
		return -ENOMEM;

	qdev->lrg_buf_q_alloc_virt_addr =
		pci_alloc_consistent(qdev->pdev,
				     qdev->lrg_buf_q_alloc_size,
				     &qdev->lrg_buf_q_alloc_phy_addr);

	if (qdev->lrg_buf_q_alloc_virt_addr == NULL) {
		netdev_err(qdev->ndev, "lBufQ failed\n");
		return -ENOMEM;
	}
	qdev->lrg_buf_q_virt_addr = qdev->lrg_buf_q_alloc_virt_addr;
	qdev->lrg_buf_q_phy_addr = qdev->lrg_buf_q_alloc_phy_addr;

	/* Create Small Buffer Queue */
	qdev->small_buf_q_size =
		NUM_SBUFQ_ENTRIES * sizeof(struct lrg_buf_q_entry);
	if (qdev->small_buf_q_size < PAGE_SIZE)
		qdev->small_buf_q_alloc_size = PAGE_SIZE;
	else
		qdev->small_buf_q_alloc_size = qdev->small_buf_q_size * 2;

	qdev->small_buf_q_alloc_virt_addr =
		pci_alloc_consistent(qdev->pdev,
				     qdev->small_buf_q_alloc_size,
				     &qdev->small_buf_q_alloc_phy_addr);

	if (qdev->small_buf_q_alloc_virt_addr == NULL) {
		netdev_err(qdev->ndev, "Small Buffer Queue allocation failed\n");
		pci_free_consistent(qdev->pdev, qdev->lrg_buf_q_alloc_size,
				    qdev->lrg_buf_q_alloc_virt_addr,
				    qdev->lrg_buf_q_alloc_phy_addr);
		return -ENOMEM;
	}

	qdev->small_buf_q_virt_addr = qdev->small_buf_q_alloc_virt_addr;
	qdev->small_buf_q_phy_addr = qdev->small_buf_q_alloc_phy_addr;
	set_bit(QL_ALLOC_BUFQS_DONE, &qdev->flags);
	return 0;