
static int ql_alloc_net_req_rsp_queues(struct ql3_adapter *qdev)
{
	qdev->req_q_size =
	    (u32) (NUM_REQ_Q_ENTRIES * sizeof(struct ob_mac_iocb_req));

	qdev->rsp_q_size = NUM_RSP_Q_ENTRIES * sizeof(struct net_rsp_iocb);

	/* The barrier is required to ensure request and response queue
	 * addr writes to the registers.
	 */
	wmb();

	qdev->req_q_virt_addr =
	    pci_alloc_consistent(qdev->pdev,
				 (size_t) qdev->req_q_size,
				 &qdev->req_q_phy_addr);

	if ((qdev->req_q_virt_addr == NULL) ||
	    LS_64BITS(qdev->req_q_phy_addr) & (qdev->req_q_size - 1)) {
		netdev_err(qdev->ndev, "reqQ failed\n");
		return -ENOMEM;
	}

	qdev->rsp_q_virt_addr =
	    pci_alloc_consistent(qdev->pdev,
				 (size_t) qdev->rsp_q_size,
				 &qdev->rsp_q_phy_addr);

	if ((qdev->rsp_q_virt_addr == NULL) ||
	    LS_64BITS(qdev->rsp_q_phy_addr) & (qdev->rsp_q_size - 1)) {
		netdev_err(qdev->ndev, "rspQ allocation failed\n");
		pci_free_consistent(qdev->pdev, (size_t) qdev->req_q_size,
				    qdev->req_q_virt_addr,
				    qdev->req_q_phy_addr);
		return -ENOMEM;
	}

	set_bit(QL_ALLOC_REQ_RSP_Q_DONE, &qdev->flags);

	return 0;