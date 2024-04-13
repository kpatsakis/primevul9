
static void ql_free_net_req_rsp_queues(struct ql3_adapter *qdev)
{
	if (!test_bit(QL_ALLOC_REQ_RSP_Q_DONE, &qdev->flags)) {
		netdev_info(qdev->ndev, "Already done\n");
		return;
	}

	pci_free_consistent(qdev->pdev,
			    qdev->req_q_size,
			    qdev->req_q_virt_addr, qdev->req_q_phy_addr);

	qdev->req_q_virt_addr = NULL;

	pci_free_consistent(qdev->pdev,
			    qdev->rsp_q_size,
			    qdev->rsp_q_virt_addr, qdev->rsp_q_phy_addr);

	qdev->rsp_q_virt_addr = NULL;

	clear_bit(QL_ALLOC_REQ_RSP_Q_DONE, &qdev->flags);