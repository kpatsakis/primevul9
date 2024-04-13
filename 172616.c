static void scrub_parity_put(struct scrub_parity *sparity)
{
	if (!refcount_dec_and_test(&sparity->refs))
		return;

	scrub_parity_check_and_repair(sparity);
}