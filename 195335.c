static void put_fc_log(struct fs_context *fc)
{
	struct fc_log *log = fc->log.log;
	int i;

	if (log) {
		if (refcount_dec_and_test(&log->usage)) {
			fc->log.log = NULL;
			for (i = 0; i <= 7; i++)
				if (log->need_free & (1 << i))
					kfree(log->buffer[i]);
			kfree(log);
		}
	}
}