static void __exit bnx2x_cleanup(void)
{
	struct list_head *pos, *q;

	pci_unregister_driver(&bnx2x_pci_driver);

	destroy_workqueue(bnx2x_wq);
	destroy_workqueue(bnx2x_iov_wq);

	/* Free globally allocated resources */
	list_for_each_safe(pos, q, &bnx2x_prev_list) {
		struct bnx2x_prev_path_list *tmp =
			list_entry(pos, struct bnx2x_prev_path_list, list);
		list_del(pos);
		kfree(tmp);
	}
}