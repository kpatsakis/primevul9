static ssize_t yurex_write(struct file *file, const char __user *user_buffer,
			   size_t count, loff_t *ppos)
{
	struct usb_yurex *dev;
	int i, set = 0, retval = 0;
	char buffer[16 + 1];
	char *data = buffer;
	unsigned long long c, c2 = 0;
	signed long timeout = 0;
	DEFINE_WAIT(wait);

	count = min(sizeof(buffer) - 1, count);
	dev = file->private_data;

	/* verify that we actually have some data to write */
	if (count == 0)
		goto error;

	mutex_lock(&dev->io_mutex);
	if (!dev->interface) {		/* already disconnected */
		mutex_unlock(&dev->io_mutex);
		retval = -ENODEV;
		goto error;
	}

	if (copy_from_user(buffer, user_buffer, count)) {
		mutex_unlock(&dev->io_mutex);
		retval = -EFAULT;
		goto error;
	}
	buffer[count] = 0;
	memset(dev->cntl_buffer, CMD_PADDING, YUREX_BUF_SIZE);

	switch (buffer[0]) {
	case CMD_ANIMATE:
	case CMD_LED:
		dev->cntl_buffer[0] = buffer[0];
		dev->cntl_buffer[1] = buffer[1];
		dev->cntl_buffer[2] = CMD_EOF;
		break;
	case CMD_READ:
	case CMD_VERSION:
		dev->cntl_buffer[0] = buffer[0];
		dev->cntl_buffer[1] = 0x00;
		dev->cntl_buffer[2] = CMD_EOF;
		break;
	case CMD_SET:
		data++;
		/* FALL THROUGH */
	case '0' ... '9':
		set = 1;
		c = c2 = simple_strtoull(data, NULL, 0);
		dev->cntl_buffer[0] = CMD_SET;
		for (i = 1; i < 6; i++) {
			dev->cntl_buffer[i] = (c>>32) & 0xff;
			c <<= 8;
		}
		buffer[6] = CMD_EOF;
		break;
	default:
		mutex_unlock(&dev->io_mutex);
		return -EINVAL;
	}

	/* send the data as the control msg */
	prepare_to_wait(&dev->waitq, &wait, TASK_INTERRUPTIBLE);
	dev_dbg(&dev->interface->dev, "%s - submit %c\n", __func__,
		dev->cntl_buffer[0]);
	retval = usb_submit_urb(dev->cntl_urb, GFP_KERNEL);
	if (retval >= 0)
		timeout = schedule_timeout(YUREX_WRITE_TIMEOUT);
	finish_wait(&dev->waitq, &wait);

	mutex_unlock(&dev->io_mutex);

	if (retval < 0) {
		dev_err(&dev->interface->dev,
			"%s - failed to send bulk msg, error %d\n",
			__func__, retval);
		goto error;
	}
	if (set && timeout)
		dev->bbu = c2;
	return timeout ? count : -EIO;

error:
	return retval;
}