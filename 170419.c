void bpf_link_put(struct bpf_link *link)
{
	if (!atomic64_dec_and_test(&link->refcnt))
		return;

	if (in_atomic()) {
		INIT_WORK(&link->work, bpf_link_put_deferred);
		schedule_work(&link->work);
	} else {
		bpf_link_free(link);
	}
}