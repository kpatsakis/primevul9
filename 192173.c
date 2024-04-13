void mce_unregister_decode_chain(struct notifier_block *nb)
{
	atomic_dec(&num_notifiers);

	blocking_notifier_chain_unregister(&x86_mce_decoder_chain, nb);
}