void __exit snd_sequencer_device_done(void)
{
	snd_unregister_device(&seq_dev);
	put_device(&seq_dev);
}