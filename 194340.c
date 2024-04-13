static long sg_compat_ioctl(struct file *filp, unsigned int cmd_in, unsigned long arg)
{
	Sg_device *sdp;
	Sg_fd *sfp;
	struct scsi_device *sdev;

	if ((!(sfp = (Sg_fd *) filp->private_data)) || (!(sdp = sfp->parentdp)))
		return -ENXIO;

	sdev = sdp->device;
	if (sdev->host->hostt->compat_ioctl) {
		int ret;

		ret = sdev->host->hostt->compat_ioctl(sdev, cmd_in, (void __user *)arg);

		return ret;
	}

	return -ENOIOCTLCMD;
}