
static void ql_free_small_buffers(struct ql3_adapter *qdev)
{
	if (!test_bit(QL_ALLOC_SMALL_BUF_DONE, &qdev->flags)) {
		netdev_info(qdev->ndev, "Already done\n");
		return;
	}
	if (qdev->small_buf_virt_addr != NULL) {
		pci_free_consistent(qdev->pdev,
				    qdev->small_buf_total_size,
				    qdev->small_buf_virt_addr,
				    qdev->small_buf_phy_addr);

		qdev->small_buf_virt_addr = NULL;
	}