static int pcan_usb_fd_drv_loaded(struct peak_usb_device *dev, bool loaded)
{
	struct pcan_usb_fd_device *pdev =
			container_of(dev, struct pcan_usb_fd_device, dev);

	pdev->cmd_buffer_addr[0] = 0;
	pdev->cmd_buffer_addr[1] = !!loaded;

	return pcan_usb_pro_send_req(dev,
				PCAN_USBPRO_REQ_FCT,
				PCAN_USBPRO_FCT_DRVLD,
				pdev->cmd_buffer_addr,
				PCAN_USBPRO_FCT_DRVLD_REQ_LEN);
}