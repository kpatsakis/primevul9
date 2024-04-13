static void pcan_usb_fd_free(struct peak_usb_device *dev)
{
	/* last device: can free shared objects now */
	if (!dev->prev_siblings && !dev->next_siblings) {
		struct pcan_usb_fd_device *pdev =
			container_of(dev, struct pcan_usb_fd_device, dev);

		/* free commands buffer */
		kfree(pdev->cmd_buffer_addr);

		/* free usb interface object */
		kfree(pdev->usb_if);
	}
}