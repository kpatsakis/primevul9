void iov_iter_revert(struct iov_iter *i, size_t unroll)
{
	if (!unroll)
		return;
	if (WARN_ON(unroll > MAX_RW_COUNT))
		return;
	i->count += unroll;
	if (unlikely(iov_iter_is_pipe(i))) {
		struct pipe_inode_info *pipe = i->pipe;
		unsigned int p_mask = pipe->ring_size - 1;
		unsigned int i_head = i->head;
		size_t off = i->iov_offset;
		while (1) {
			struct pipe_buffer *b = &pipe->bufs[i_head & p_mask];
			size_t n = off - b->offset;
			if (unroll < n) {
				off -= unroll;
				break;
			}
			unroll -= n;
			if (!unroll && i_head == i->start_head) {
				off = 0;
				break;
			}
			i_head--;
			b = &pipe->bufs[i_head & p_mask];
			off = b->offset + b->len;
		}
		i->iov_offset = off;
		i->head = i_head;
		pipe_truncate(i);
		return;
	}
	if (unlikely(iov_iter_is_discard(i)))
		return;
	if (unroll <= i->iov_offset) {
		i->iov_offset -= unroll;
		return;
	}
	unroll -= i->iov_offset;
	if (iov_iter_is_xarray(i)) {
		BUG(); /* We should never go beyond the start of the specified
			* range since we might then be straying into pages that
			* aren't pinned.
			*/
	} else if (iov_iter_is_bvec(i)) {
		const struct bio_vec *bvec = i->bvec;
		while (1) {
			size_t n = (--bvec)->bv_len;
			i->nr_segs++;
			if (unroll <= n) {
				i->bvec = bvec;
				i->iov_offset = n - unroll;
				return;
			}
			unroll -= n;
		}
	} else { /* same logics for iovec and kvec */
		const struct iovec *iov = i->iov;
		while (1) {
			size_t n = (--iov)->iov_len;
			i->nr_segs++;
			if (unroll <= n) {
				i->iov = iov;
				i->iov_offset = n - unroll;
				return;
			}
			unroll -= n;
		}
	}
}