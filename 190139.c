int aac_do_ioctl(struct aac_dev * dev, int cmd, void __user *arg)
{
	int status;

	mutex_lock(&dev->ioctl_mutex);

	if (dev->adapter_shutdown) {
		status = -EACCES;
		goto cleanup;
	}

	/*
	 *	HBA gets first crack
	 */

	status = aac_dev_ioctl(dev, cmd, arg);
	if (status != -ENOTTY)
		goto cleanup;

	switch (cmd) {
	case FSACTL_MINIPORT_REV_CHECK:
		status = check_revision(dev, arg);
		break;
	case FSACTL_SEND_LARGE_FIB:
	case FSACTL_SENDFIB:
		status = ioctl_send_fib(dev, arg);
		break;
	case FSACTL_OPEN_GET_ADAPTER_FIB:
		status = open_getadapter_fib(dev, arg);
		break;
	case FSACTL_GET_NEXT_ADAPTER_FIB:
		status = next_getadapter_fib(dev, arg);
		break;
	case FSACTL_CLOSE_GET_ADAPTER_FIB:
		status = close_getadapter_fib(dev, arg);
		break;
	case FSACTL_SEND_RAW_SRB:
		status = aac_send_raw_srb(dev,arg);
		break;
	case FSACTL_GET_PCI_INFO:
		status = aac_get_pci_info(dev,arg);
		break;
	default:
		status = -ENOTTY;
		break;
	}

cleanup:
	mutex_unlock(&dev->ioctl_mutex);

	return status;
}