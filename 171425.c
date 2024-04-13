
static void ql_free_buffer_queues(struct ql3_adapter *qdev)
{
	if (!test_bit(QL_ALLOC_BUFQS_DONE, &qdev->flags)) {
		netdev_info(qdev->ndev, "Already done\n");
		return;
	}
	kfree(qdev->lrg_buf);
	pci_free_consistent(qdev->pdev,
			    qdev->lrg_buf_q_alloc_size,
			    qdev->lrg_buf_q_alloc_virt_addr,
			    qdev->lrg_buf_q_alloc_phy_addr);

	qdev->lrg_buf_q_virt_addr = NULL;

	pci_free_consistent(qdev->pdev,
			    qdev->small_buf_q_alloc_size,
			    qdev->small_buf_q_alloc_virt_addr,
			    qdev->small_buf_q_alloc_phy_addr);

	qdev->small_buf_q_virt_addr = NULL;

	clear_bit(QL_ALLOC_BUFQS_DONE, &qdev->flags);