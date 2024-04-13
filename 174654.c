vcs_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(file);
	struct vc_data *vc;
	struct vcs_poll_data *poll;
	long pos, read;
	int attr, uni_mode, row, col, maxcol, viewed;
	unsigned short *org = NULL;
	ssize_t ret;
	char *con_buf;

	con_buf = (char *) __get_free_page(GFP_KERNEL);
	if (!con_buf)
		return -ENOMEM;

	pos = *ppos;

	/* Select the proper current console and verify
	 * sanity of the situation under the console lock.
	 */
	console_lock();

	uni_mode = use_unicode(inode);
	attr = use_attributes(inode);
	ret = -ENXIO;
	vc = vcs_vc(inode, &viewed);
	if (!vc)
		goto unlock_out;

	ret = -EINVAL;
	if (pos < 0)
		goto unlock_out;
	/* we enforce 32-bit alignment for pos and count in unicode mode */
	if (uni_mode && (pos | count) & 3)
		goto unlock_out;

	poll = file->private_data;
	if (count && poll)
		poll->event = 0;
	read = 0;
	ret = 0;
	while (count) {
		char *con_buf0, *con_buf_start;
		long this_round, size;
		ssize_t orig_count;
		long p = pos;

		/* Check whether we are above size each round,
		 * as copy_to_user at the end of this loop
		 * could sleep.
		 */
		size = vcs_size(inode);
		if (size < 0) {
			if (read)
				break;
			ret = size;
			goto unlock_out;
		}
		if (pos >= size)
			break;
		if (count > size - pos)
			count = size - pos;

		this_round = count;
		if (this_round > CON_BUF_SIZE)
			this_round = CON_BUF_SIZE;

		/* Perform the whole read into the local con_buf.
		 * Then we can drop the console spinlock and safely
		 * attempt to move it to userspace.
		 */

		con_buf_start = con_buf0 = con_buf;
		orig_count = this_round;
		maxcol = vc->vc_cols;
		if (uni_mode) {
			unsigned int nr;

			ret = vc_uniscr_check(vc);
			if (ret)
				break;
			p /= 4;
			row = p / vc->vc_cols;
			col = p % maxcol;
			nr = maxcol - col;
			do {
				if (nr > this_round/4)
					nr = this_round/4;
				vc_uniscr_copy_line(vc, con_buf0, viewed,
						    row, col, nr);
				con_buf0 += nr * 4;
				this_round -= nr * 4;
				row++;
				col = 0;
				nr = maxcol;
			} while (this_round);
		} else if (!attr) {
			org = screen_pos(vc, p, viewed);
			col = p % maxcol;
			p += maxcol - col;
			while (this_round-- > 0) {
				*con_buf0++ = (vcs_scr_readw(vc, org++) & 0xff);
				if (++col == maxcol) {
					org = screen_pos(vc, p, viewed);
					col = 0;
					p += maxcol;
				}
			}
		} else {
			if (p < HEADER_SIZE) {
				size_t tmp_count;

				/* clamp header values if they don't fit */
				con_buf0[0] = min(vc->vc_rows, 0xFFu);
				con_buf0[1] = min(vc->vc_cols, 0xFFu);
				getconsxy(vc, con_buf0 + 2);

				con_buf_start += p;
				this_round += p;
				if (this_round > CON_BUF_SIZE) {
					this_round = CON_BUF_SIZE;
					orig_count = this_round - p;
				}

				tmp_count = HEADER_SIZE;
				if (tmp_count > this_round)
					tmp_count = this_round;

				/* Advance state pointers and move on. */
				this_round -= tmp_count;
				p = HEADER_SIZE;
				con_buf0 = con_buf + HEADER_SIZE;
				/* If this_round >= 0, then p is even... */
			} else if (p & 1) {
				/* Skip first byte for output if start address is odd
				 * Update region sizes up/down depending on free
				 * space in buffer.
				 */
				con_buf_start++;
				if (this_round < CON_BUF_SIZE)
					this_round++;
				else
					orig_count--;
			}
			if (this_round > 0) {
				unsigned short *tmp_buf = (unsigned short *)con_buf0;

				p -= HEADER_SIZE;
				p /= 2;
				col = p % maxcol;

				org = screen_pos(vc, p, viewed);
				p += maxcol - col;

				/* Buffer has even length, so we can always copy
				 * character + attribute. We do not copy last byte
				 * to userspace if this_round is odd.
				 */
				this_round = (this_round + 1) >> 1;

				while (this_round) {
					*tmp_buf++ = vcs_scr_readw(vc, org++);
					this_round --;
					if (++col == maxcol) {
						org = screen_pos(vc, p, viewed);
						col = 0;
						p += maxcol;
					}
				}
			}
		}

		/* Finally, release the console semaphore while we push
		 * all the data to userspace from our temporary buffer.
		 *
		 * AKPM: Even though it's a semaphore, we should drop it because
		 * the pagefault handling code may want to call printk().
		 */

		console_unlock();
		ret = copy_to_user(buf, con_buf_start, orig_count);
		console_lock();

		if (ret) {
			read += (orig_count - ret);
			ret = -EFAULT;
			break;
		}
		buf += orig_count;
		pos += orig_count;
		read += orig_count;
		count -= orig_count;
	}
	*ppos += read;
	if (read)
		ret = read;
unlock_out:
	console_unlock();
	free_page((unsigned long) con_buf);
	return ret;
}