static ssize_t intel_vgpu_read(struct mdev_device *mdev, char __user *buf,
			size_t count, loff_t *ppos)
{
	unsigned int done = 0;
	int ret;

	while (count) {
		size_t filled;

		/* Only support GGTT entry 8 bytes read */
		if (count >= 8 && !(*ppos % 8) &&
			gtt_entry(mdev, ppos)) {
			u64 val;

			ret = intel_vgpu_rw(mdev, (char *)&val, sizeof(val),
					ppos, false);
			if (ret <= 0)
				goto read_err;

			if (copy_to_user(buf, &val, sizeof(val)))
				goto read_err;

			filled = 8;
		} else if (count >= 4 && !(*ppos % 4)) {
			u32 val;

			ret = intel_vgpu_rw(mdev, (char *)&val, sizeof(val),
					ppos, false);
			if (ret <= 0)
				goto read_err;

			if (copy_to_user(buf, &val, sizeof(val)))
				goto read_err;

			filled = 4;
		} else if (count >= 2 && !(*ppos % 2)) {
			u16 val;

			ret = intel_vgpu_rw(mdev, (char *)&val, sizeof(val),
					ppos, false);
			if (ret <= 0)
				goto read_err;

			if (copy_to_user(buf, &val, sizeof(val)))
				goto read_err;

			filled = 2;
		} else {
			u8 val;

			ret = intel_vgpu_rw(mdev, &val, sizeof(val), ppos,
					false);
			if (ret <= 0)
				goto read_err;

			if (copy_to_user(buf, &val, sizeof(val)))
				goto read_err;

			filled = 1;
		}

		count -= filled;
		done += filled;
		*ppos += filled;
		buf += filled;
	}

	return done;

read_err:
	return -EFAULT;
}