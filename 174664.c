vcs_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);
	struct vc_data *vc;
	long pos;
	long attr, size, written;
	char *con_buf0;
	int col, maxcol, viewed;
	u16 *org0 = NULL, *org = NULL;
	size_t ret;
	char *con_buf;

	if (use_unicode(inode))
		return -EOPNOTSUPP;

	con_buf = (char *) __get_free_page(GFP_KERNEL);
	if (!con_buf)
		return -ENOMEM;

	pos = *ppos;

	/* Select the proper current console and verify
	 * sanity of the situation under the console lock.
	 */
	console_lock();

	attr = use_attributes(inode);
	ret = -ENXIO;
	vc = vcs_vc(inode, &viewed);
	if (!vc)
		goto unlock_out;

	size = vcs_size(inode);
	ret = -EINVAL;
	if (pos < 0 || pos > size)
		goto unlock_out;
	if (count > size - pos)
		count = size - pos;
	written = 0;
	while (count) {
		long this_round = count;
		size_t orig_count;
		long p;

		if (this_round > CON_BUF_SIZE)
			this_round = CON_BUF_SIZE;

		/* Temporarily drop the console lock so that we can read
		 * in the write data from userspace safely.
		 */
		console_unlock();
		ret = copy_from_user(con_buf, buf, this_round);
		console_lock();

		if (ret) {
			this_round -= ret;
			if (!this_round) {
				/* Abort loop if no data were copied. Otherwise
				 * fail with -EFAULT.
				 */
				if (written)
					break;
				ret = -EFAULT;
				goto unlock_out;
			}
		}

		/* The vcs_size might have changed while we slept to grab
		 * the user buffer, so recheck.
		 * Return data written up to now on failure.
		 */
		size = vcs_size(inode);
		if (size < 0) {
			if (written)
				break;
			ret = size;
			goto unlock_out;
		}
		if (pos >= size)
			break;
		if (this_round > size - pos)
			this_round = size - pos;

		/* OK, now actually push the write to the console
		 * under the lock using the local kernel buffer.
		 */

		con_buf0 = con_buf;
		orig_count = this_round;
		maxcol = vc->vc_cols;
		p = pos;
		if (!attr) {
			org0 = org = screen_pos(vc, p, viewed);
			col = p % maxcol;
			p += maxcol - col;

			while (this_round > 0) {
				unsigned char c = *con_buf0++;

				this_round--;
				vcs_scr_writew(vc,
					       (vcs_scr_readw(vc, org) & 0xff00) | c, org);
				org++;
				if (++col == maxcol) {
					org = screen_pos(vc, p, viewed);
					col = 0;
					p += maxcol;
				}
			}
		} else {
			if (p < HEADER_SIZE) {
				char header[HEADER_SIZE];

				getconsxy(vc, header + 2);
				while (p < HEADER_SIZE && this_round > 0) {
					this_round--;
					header[p++] = *con_buf0++;
				}
				if (!viewed)
					putconsxy(vc, header + 2);
			}
			p -= HEADER_SIZE;
			col = (p/2) % maxcol;
			if (this_round > 0) {
				org0 = org = screen_pos(vc, p/2, viewed);
				if ((p & 1) && this_round > 0) {
					char c;

					this_round--;
					c = *con_buf0++;
#ifdef __BIG_ENDIAN
					vcs_scr_writew(vc, c |
					     (vcs_scr_readw(vc, org) & 0xff00), org);
#else
					vcs_scr_writew(vc, (c << 8) |
					     (vcs_scr_readw(vc, org) & 0xff), org);
#endif
					org++;
					p++;
					if (++col == maxcol) {
						org = screen_pos(vc, p/2, viewed);
						col = 0;
					}
				}
				p /= 2;
				p += maxcol - col;
			}
			while (this_round > 1) {
				unsigned short w;

				w = get_unaligned(((unsigned short *)con_buf0));
				vcs_scr_writew(vc, w, org++);
				con_buf0 += 2;
				this_round -= 2;
				if (++col == maxcol) {
					org = screen_pos(vc, p, viewed);
					col = 0;
					p += maxcol;
				}
			}
			if (this_round > 0) {
				unsigned char c;

				c = *con_buf0++;
#ifdef __BIG_ENDIAN
				vcs_scr_writew(vc, (vcs_scr_readw(vc, org) & 0xff) | (c << 8), org);
#else
				vcs_scr_writew(vc, (vcs_scr_readw(vc, org) & 0xff00) | c, org);
#endif
			}
		}
		count -= orig_count;
		written += orig_count;
		buf += orig_count;
		pos += orig_count;
		if (org0)
			update_region(vc, (unsigned long)(org0), org - org0);
	}
	*ppos += written;
	ret = written;
	if (written)
		vcs_scr_updated(vc);

unlock_out:
	console_unlock();
	free_page((unsigned long) con_buf);
	return ret;
}