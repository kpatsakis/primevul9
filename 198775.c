static int unregister_gadget(struct gadget_info *gi)
{
	int ret;

	if (!gi->composite.gadget_driver.udc_name)
		return -ENODEV;

	ret = usb_gadget_unregister_driver(&gi->composite.gadget_driver);
	if (ret)
		return ret;
	kfree(gi->composite.gadget_driver.udc_name);
	gi->composite.gadget_driver.udc_name = NULL;
	return 0;
}