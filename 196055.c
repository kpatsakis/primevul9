write_rio(struct file *file, const char __user *buffer,
	  size_t count, loff_t * ppos)
{
	DEFINE_WAIT(wait);
	struct rio_usb_data *rio = &rio_instance;

	unsigned long copy_size;
	unsigned long bytes_written = 0;
	unsigned int partial;

	int result = 0;
	int maxretry;
	int errn = 0;
	int intr;

	intr = mutex_lock_interruptible(&(rio->lock));
	if (intr)
		return -EINTR;
        /* Sanity check to make sure rio is connected, powered, etc */
        if (rio->present == 0 || rio->rio_dev == NULL) {
		mutex_unlock(&(rio->lock));
		return -ENODEV;
	}



	do {
		unsigned long thistime;
		char *obuf = rio->obuf;

		thistime = copy_size =
		    (count >= OBUF_SIZE) ? OBUF_SIZE : count;
		if (copy_from_user(rio->obuf, buffer, copy_size)) {
			errn = -EFAULT;
			goto error;
		}
		maxretry = 5;
		while (thistime) {
			if (!rio->rio_dev) {
				errn = -ENODEV;
				goto error;
			}
			if (signal_pending(current)) {
				mutex_unlock(&(rio->lock));
				return bytes_written ? bytes_written : -EINTR;
			}

			result = usb_bulk_msg(rio->rio_dev,
					 usb_sndbulkpipe(rio->rio_dev, 2),
					 obuf, thistime, &partial, 5000);

			dev_dbg(&rio->rio_dev->dev,
				"write stats: result:%d thistime:%lu partial:%u\n",
				result, thistime, partial);

			if (result == -ETIMEDOUT) {	/* NAK - so hold for a while */
				if (!maxretry--) {
					errn = -ETIME;
					goto error;
				}
				prepare_to_wait(&rio->wait_q, &wait, TASK_INTERRUPTIBLE);
				schedule_timeout(NAK_TIMEOUT);
				finish_wait(&rio->wait_q, &wait);
				continue;
			} else if (!result && partial) {
				obuf += partial;
				thistime -= partial;
			} else
				break;
		}
		if (result) {
			dev_err(&rio->rio_dev->dev, "Write Whoops - %x\n",
				result);
			errn = -EIO;
			goto error;
		}
		bytes_written += copy_size;
		count -= copy_size;
		buffer += copy_size;
	} while (count > 0);

	mutex_unlock(&(rio->lock));

	return bytes_written ? bytes_written : -EIO;

error:
	mutex_unlock(&(rio->lock));
	return errn;
}