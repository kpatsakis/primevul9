static int __init rtl8xxxu_module_init(void)
{
	int res;

	res = usb_register(&rtl8xxxu_driver);
	if (res < 0)
		pr_err(DRIVER_NAME ": usb_register() failed (%i)\n", res);

	return res;
}