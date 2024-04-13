static inline void *pcan_usb_fd_cmd_buffer(struct peak_usb_device *dev)
{
	struct pcan_usb_fd_device *pdev =
			container_of(dev, struct pcan_usb_fd_device, dev);
	return pdev->cmd_buffer_addr;
}