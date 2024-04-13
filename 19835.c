ssize_t seq_read_iter(struct kiocb *iocb, struct iov_iter *iter)
{
	struct seq_file *m = iocb->ki_filp->private_data;
	size_t copied = 0;
	size_t n;
	void *p;
	int err = 0;

	if (!iov_iter_count(iter))
		return 0;

	mutex_lock(&m->lock);

	/*
	 * if request is to read from zero offset, reset iterator to first
	 * record as it might have been already advanced by previous requests
	 */
	if (iocb->ki_pos == 0) {
		m->index = 0;
		m->count = 0;
	}

	/* Don't assume ki_pos is where we left it */
	if (unlikely(iocb->ki_pos != m->read_pos)) {
		while ((err = traverse(m, iocb->ki_pos)) == -EAGAIN)
			;
		if (err) {
			/* With prejudice... */
			m->read_pos = 0;
			m->index = 0;
			m->count = 0;
			goto Done;
		} else {
			m->read_pos = iocb->ki_pos;
		}
	}

	/* grab buffer if we didn't have one */
	if (!m->buf) {
		m->buf = seq_buf_alloc(m->size = PAGE_SIZE);
		if (!m->buf)
			goto Enomem;
	}
	// something left in the buffer - copy it out first
	if (m->count) {
		n = copy_to_iter(m->buf + m->from, m->count, iter);
		m->count -= n;
		m->from += n;
		copied += n;
		if (m->count)	// hadn't managed to copy everything
			goto Done;
	}
	// get a non-empty record in the buffer
	m->from = 0;
	p = m->op->start(m, &m->index);
	while (1) {
		err = PTR_ERR(p);
		if (!p || IS_ERR(p))	// EOF or an error
			break;
		err = m->op->show(m, p);
		if (err < 0)		// hard error
			break;
		if (unlikely(err))	// ->show() says "skip it"
			m->count = 0;
		if (unlikely(!m->count)) { // empty record
			p = m->op->next(m, p, &m->index);
			continue;
		}
		if (!seq_has_overflowed(m)) // got it
			goto Fill;
		// need a bigger buffer
		m->op->stop(m, p);
		kvfree(m->buf);
		m->count = 0;
		m->buf = seq_buf_alloc(m->size <<= 1);
		if (!m->buf)
			goto Enomem;
		p = m->op->start(m, &m->index);
	}
	// EOF or an error
	m->op->stop(m, p);
	m->count = 0;
	goto Done;
Fill:
	// one non-empty record is in the buffer; if they want more,
	// try to fit more in, but in any case we need to advance
	// the iterator once for every record shown.
	while (1) {
		size_t offs = m->count;
		loff_t pos = m->index;

		p = m->op->next(m, p, &m->index);
		if (pos == m->index) {
			pr_info_ratelimited("buggy .next function %ps did not update position index\n",
					    m->op->next);
			m->index++;
		}
		if (!p || IS_ERR(p))	// no next record for us
			break;
		if (m->count >= iov_iter_count(iter))
			break;
		err = m->op->show(m, p);
		if (err > 0) {		// ->show() says "skip it"
			m->count = offs;
		} else if (err || seq_has_overflowed(m)) {
			m->count = offs;
			break;
		}
	}
	m->op->stop(m, p);
	n = copy_to_iter(m->buf, m->count, iter);
	copied += n;
	m->count -= n;
	m->from = n;
Done:
	if (unlikely(!copied)) {
		copied = m->count ? -EFAULT : err;
	} else {
		iocb->ki_pos += copied;
		m->read_pos += copied;
	}
	mutex_unlock(&m->lock);
	return copied;
Enomem:
	err = -ENOMEM;
	goto Done;
}