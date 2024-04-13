base_sock_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	int			err = 0, id;
	struct mISDNdevice	*dev;
	struct mISDNversion	ver;

	switch (cmd) {
	case IMGETVERSION:
		ver.major = MISDN_MAJOR_VERSION;
		ver.minor = MISDN_MINOR_VERSION;
		ver.release = MISDN_RELEASE;
		if (copy_to_user((void __user *)arg, &ver, sizeof(ver)))
			err = -EFAULT;
		break;
	case IMGETCOUNT:
		id = get_mdevice_count();
		if (put_user(id, (int __user *)arg))
			err = -EFAULT;
		break;
	case IMGETDEVINFO:
		if (get_user(id, (int __user *)arg)) {
			err = -EFAULT;
			break;
		}
		dev = get_mdevice(id);
		if (dev) {
			struct mISDN_devinfo di;

			memset(&di, 0, sizeof(di));
			di.id = dev->id;
			di.Dprotocols = dev->Dprotocols;
			di.Bprotocols = dev->Bprotocols | get_all_Bprotocols();
			di.protocol = dev->D.protocol;
			memcpy(di.channelmap, dev->channelmap,
			       sizeof(di.channelmap));
			di.nrbchan = dev->nrbchan;
			strscpy(di.name, dev_name(&dev->dev), sizeof(di.name));
			if (copy_to_user((void __user *)arg, &di, sizeof(di)))
				err = -EFAULT;
		} else
			err = -ENODEV;
		break;
	case IMSETDEVNAME:
	{
		struct mISDN_devrename dn;
		if (copy_from_user(&dn, (void __user *)arg,
				   sizeof(dn))) {
			err = -EFAULT;
			break;
		}
		dn.name[sizeof(dn.name) - 1] = '\0';
		dev = get_mdevice(dn.id);
		if (dev)
			err = device_rename(&dev->dev, dn.name);
		else
			err = -ENODEV;
	}
	break;
	default:
		err = -EINVAL;
	}
	return err;
}