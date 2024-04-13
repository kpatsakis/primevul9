static void __exit rtl8xxxu_module_exit(void)
{
	usb_deregister(&rtl8xxxu_driver);
}