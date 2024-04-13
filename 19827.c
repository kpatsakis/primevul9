int seq_write(struct seq_file *seq, const void *data, size_t len)
{
	if (seq->count + len < seq->size) {
		memcpy(seq->buf + seq->count, data, len);
		seq->count += len;
		return 0;
	}
	seq_set_overflow(seq);
	return -1;
}