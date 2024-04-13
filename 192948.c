static ssize_t snd_seq_read(struct file *file, char __user *buf, size_t count,
			    loff_t *offset)
{
	struct snd_seq_client *client = file->private_data;
	struct snd_seq_fifo *fifo;
	int err;
	long result = 0;
	struct snd_seq_event_cell *cell;

	if (!(snd_seq_file_flags(file) & SNDRV_SEQ_LFLG_INPUT))
		return -ENXIO;

	if (!access_ok(VERIFY_WRITE, buf, count))
		return -EFAULT;

	/* check client structures are in place */
	if (snd_BUG_ON(!client))
		return -ENXIO;

	if (!client->accept_input || (fifo = client->data.user.fifo) == NULL)
		return -ENXIO;

	if (atomic_read(&fifo->overflow) > 0) {
		/* buffer overflow is detected */
		snd_seq_fifo_clear(fifo);
		/* return error code */
		return -ENOSPC;
	}

	cell = NULL;
	err = 0;
	snd_seq_fifo_lock(fifo);

	/* while data available in queue */
	while (count >= sizeof(struct snd_seq_event)) {
		int nonblock;

		nonblock = (file->f_flags & O_NONBLOCK) || result > 0;
		if ((err = snd_seq_fifo_cell_out(fifo, &cell, nonblock)) < 0) {
			break;
		}
		if (snd_seq_ev_is_variable(&cell->event)) {
			struct snd_seq_event tmpev;
			tmpev = cell->event;
			tmpev.data.ext.len &= ~SNDRV_SEQ_EXT_MASK;
			if (copy_to_user(buf, &tmpev, sizeof(struct snd_seq_event))) {
				err = -EFAULT;
				break;
			}
			count -= sizeof(struct snd_seq_event);
			buf += sizeof(struct snd_seq_event);
			err = snd_seq_expand_var_event(&cell->event, count,
						       (char __force *)buf, 0,
						       sizeof(struct snd_seq_event));
			if (err < 0)
				break;
			result += err;
			count -= err;
			buf += err;
		} else {
			if (copy_to_user(buf, &cell->event, sizeof(struct snd_seq_event))) {
				err = -EFAULT;
				break;
			}
			count -= sizeof(struct snd_seq_event);
			buf += sizeof(struct snd_seq_event);
		}
		snd_seq_cell_free(cell);
		cell = NULL; /* to be sure */
		result += sizeof(struct snd_seq_event);
	}

	if (err < 0) {
		if (cell)
			snd_seq_fifo_cell_putback(fifo, cell);
		if (err == -EAGAIN && result > 0)
			err = 0;
	}
	snd_seq_fifo_unlock(fifo);

	return (err < 0) ? err : result;
}