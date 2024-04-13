
static void ql_free_mem_resources(struct ql3_adapter *qdev)
{
	ql_free_send_free_list(qdev);
	ql_free_large_buffers(qdev);
	ql_free_small_buffers(qdev);
	ql_free_buffer_queues(qdev);
	ql_free_net_req_rsp_queues(qdev);
	if (qdev->shadow_reg_virt_addr != NULL) {
		pci_free_consistent(qdev->pdev,
				    PAGE_SIZE,
				    qdev->shadow_reg_virt_addr,
				    qdev->shadow_reg_phy_addr);
		qdev->shadow_reg_virt_addr = NULL;
	}