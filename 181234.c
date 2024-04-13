static ssize_t _nfsd_copy_file_range(struct nfsd4_copy *copy)
{
	ssize_t bytes_copied = 0;
	size_t bytes_total = copy->cp_count;
	u64 src_pos = copy->cp_src_pos;
	u64 dst_pos = copy->cp_dst_pos;

	do {
		if (kthread_should_stop())
			break;
		bytes_copied = nfsd_copy_file_range(copy->file_src, src_pos,
				copy->file_dst, dst_pos, bytes_total);
		if (bytes_copied <= 0)
			break;
		bytes_total -= bytes_copied;
		copy->cp_res.wr_bytes_written += bytes_copied;
		src_pos += bytes_copied;
		dst_pos += bytes_copied;
	} while (bytes_total > 0 && !copy->cp_synchronous);
	return bytes_copied;
}