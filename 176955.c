static int __init qrtr_tun_init(void)
{
	int ret;

	ret = misc_register(&qrtr_tun_miscdev);
	if (ret)
		pr_err("failed to register Qualcomm IPC Router tun device\n");

	return ret;
}