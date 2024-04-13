static int __init bnx2x_init(void)
{
	int ret;

	pr_info("%s", version);

	bnx2x_wq = create_singlethread_workqueue("bnx2x");
	if (bnx2x_wq == NULL) {
		pr_err("Cannot create workqueue\n");
		return -ENOMEM;
	}
	bnx2x_iov_wq = create_singlethread_workqueue("bnx2x_iov");
	if (!bnx2x_iov_wq) {
		pr_err("Cannot create iov workqueue\n");
		destroy_workqueue(bnx2x_wq);
		return -ENOMEM;
	}

	ret = pci_register_driver(&bnx2x_pci_driver);
	if (ret) {
		pr_err("Cannot register driver\n");
		destroy_workqueue(bnx2x_wq);
		destroy_workqueue(bnx2x_iov_wq);
	}
	return ret;
}