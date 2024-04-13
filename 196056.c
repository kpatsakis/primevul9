static long ioctl_rio(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct RioCommand rio_cmd;
	struct rio_usb_data *rio = &rio_instance;
	void __user *data;
	unsigned char *buffer;
	int result, requesttype;
	int retries;
	int retval=0;

	mutex_lock(&(rio->lock));
        /* Sanity check to make sure rio is connected, powered, etc */
        if (rio->present == 0 || rio->rio_dev == NULL) {
		retval = -ENODEV;
		goto err_out;
	}

	switch (cmd) {
	case RIO_RECV_COMMAND:
		data = (void __user *) arg;
		if (data == NULL)
			break;
		if (copy_from_user(&rio_cmd, data, sizeof(struct RioCommand))) {
			retval = -EFAULT;
			goto err_out;
		}
		if (rio_cmd.length < 0 || rio_cmd.length > PAGE_SIZE) {
			retval = -EINVAL;
			goto err_out;
		}
		buffer = (unsigned char *) __get_free_page(GFP_KERNEL);
		if (buffer == NULL) {
			retval = -ENOMEM;
			goto err_out;
		}
		if (copy_from_user(buffer, rio_cmd.buffer, rio_cmd.length)) {
			retval = -EFAULT;
			free_page((unsigned long) buffer);
			goto err_out;
		}

		requesttype = rio_cmd.requesttype | USB_DIR_IN |
		    USB_TYPE_VENDOR | USB_RECIP_DEVICE;
		dev_dbg(&rio->rio_dev->dev,
			"sending command:reqtype=%0x req=%0x value=%0x index=%0x len=%0x\n",
			requesttype, rio_cmd.request, rio_cmd.value,
			rio_cmd.index, rio_cmd.length);
		/* Send rio control message */
		retries = 3;
		while (retries) {
			result = usb_control_msg(rio->rio_dev,
						 usb_rcvctrlpipe(rio-> rio_dev, 0),
						 rio_cmd.request,
						 requesttype,
						 rio_cmd.value,
						 rio_cmd.index, buffer,
						 rio_cmd.length,
						 jiffies_to_msecs(rio_cmd.timeout));
			if (result == -ETIMEDOUT)
				retries--;
			else if (result < 0) {
				dev_err(&rio->rio_dev->dev,
					"Error executing ioctrl. code = %d\n",
					result);
				retries = 0;
			} else {
				dev_dbg(&rio->rio_dev->dev,
					"Executed ioctl. Result = %d (data=%02x)\n",
					result, buffer[0]);
				if (copy_to_user(rio_cmd.buffer, buffer,
						 rio_cmd.length)) {
					free_page((unsigned long) buffer);
					retval = -EFAULT;
					goto err_out;
				}
				retries = 0;
			}

			/* rio_cmd.buffer contains a raw stream of single byte
			   data which has been returned from rio.  Data is
			   interpreted at application level.  For data that
			   will be cast to data types longer than 1 byte, data
			   will be little_endian and will potentially need to
			   be swapped at the app level */

		}
		free_page((unsigned long) buffer);
		break;

	case RIO_SEND_COMMAND:
		data = (void __user *) arg;
		if (data == NULL)
			break;
		if (copy_from_user(&rio_cmd, data, sizeof(struct RioCommand))) {
			retval = -EFAULT;
			goto err_out;
		}
		if (rio_cmd.length < 0 || rio_cmd.length > PAGE_SIZE) {
			retval = -EINVAL;
			goto err_out;
		}
		buffer = (unsigned char *) __get_free_page(GFP_KERNEL);
		if (buffer == NULL) {
			retval = -ENOMEM;
			goto err_out;
		}
		if (copy_from_user(buffer, rio_cmd.buffer, rio_cmd.length)) {
			free_page((unsigned long)buffer);
			retval = -EFAULT;
			goto err_out;
		}

		requesttype = rio_cmd.requesttype | USB_DIR_OUT |
		    USB_TYPE_VENDOR | USB_RECIP_DEVICE;
		dev_dbg(&rio->rio_dev->dev,
			"sending command: reqtype=%0x req=%0x value=%0x index=%0x len=%0x\n",
			requesttype, rio_cmd.request, rio_cmd.value,
			rio_cmd.index, rio_cmd.length);
		/* Send rio control message */
		retries = 3;
		while (retries) {
			result = usb_control_msg(rio->rio_dev,
						 usb_sndctrlpipe(rio-> rio_dev, 0),
						 rio_cmd.request,
						 requesttype,
						 rio_cmd.value,
						 rio_cmd.index, buffer,
						 rio_cmd.length,
						 jiffies_to_msecs(rio_cmd.timeout));
			if (result == -ETIMEDOUT)
				retries--;
			else if (result < 0) {
				dev_err(&rio->rio_dev->dev,
					"Error executing ioctrl. code = %d\n",
					result);
				retries = 0;
			} else {
				dev_dbg(&rio->rio_dev->dev,
					"Executed ioctl. Result = %d\n", result);
				retries = 0;

			}

		}
		free_page((unsigned long) buffer);
		break;

	default:
		retval = -ENOTTY;
		break;
	}


err_out:
	mutex_unlock(&(rio->lock));
	return retval;
}