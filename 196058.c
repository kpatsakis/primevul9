read_rio(struct file *file, char __user *buffer, size_t count, loff_t * ppos)
{
	DEFINE_WAIT(wait);
	struct rio_usb_data *rio = &rio_instance;
	ssize_t read_count;
	unsigned int partial;
	int this_read;
	int result;
	int maxretry = 10;
	char *ibuf;
	int intr;

	intr = mutex_lock_interruptible(&(rio->lock));
	if (intr)
		return -EINTR;
	/* Sanity check to make sure rio is connected, powered, etc */
        if (rio->present == 0 || rio->rio_dev == NULL) {
		mutex_unlock(&(rio->lock));
		return -ENODEV;
	}

	ibuf = rio->ibuf;

	read_count = 0;


	while (count > 0) {
		if (signal_pending(current)) {
			mutex_unlock(&(rio->lock));
			return read_count ? read_count : -EINTR;
		}
		if (!rio->rio_dev) {
			mutex_unlock(&(rio->lock));
			return -ENODEV;
		}
		this_read = (count >= IBUF_SIZE) ? IBUF_SIZE : count;

		result = usb_bulk_msg(rio->rio_dev,
				      usb_rcvbulkpipe(rio->rio_dev, 1),
				      ibuf, this_read, &partial,
				      8000);

		dev_dbg(&rio->rio_dev->dev,
			"read stats: result:%d this_read:%u partial:%u\n",
			result, this_read, partial);

		if (partial) {
			count = this_read = partial;
		} else if (result == -ETIMEDOUT || result == 15) {	/* FIXME: 15 ??? */
			if (!maxretry--) {
				mutex_unlock(&(rio->lock));
				dev_err(&rio->rio_dev->dev,
					"read_rio: maxretry timeout\n");
				return -ETIME;
			}
			prepare_to_wait(&rio->wait_q, &wait, TASK_INTERRUPTIBLE);
			schedule_timeout(NAK_TIMEOUT);
			finish_wait(&rio->wait_q, &wait);
			continue;
		} else if (result != -EREMOTEIO) {
			mutex_unlock(&(rio->lock));
			dev_err(&rio->rio_dev->dev,
				"Read Whoops - result:%d partial:%u this_read:%u\n",
				result, partial, this_read);
			return -EIO;
		} else {
			mutex_unlock(&(rio->lock));
			return (0);
		}

		if (this_read) {
			if (copy_to_user(buffer, ibuf, this_read)) {
				mutex_unlock(&(rio->lock));
				return -EFAULT;
			}
			count -= this_read;
			read_count += this_read;
			buffer += this_read;
		}
	}
	mutex_unlock(&(rio->lock));
	return read_count;
}