void mce_register_decode_chain(struct notifier_block *nb)
{
	if (WARN_ON(nb->priority > MCE_PRIO_MCELOG && nb->priority < MCE_PRIO_EDAC))
		return;

	atomic_inc(&num_notifiers);

	blocking_notifier_chain_register(&x86_mce_decoder_chain, nb);
}