int __init snd_sequencer_device_init(void)
{
	int err;

	snd_device_initialize(&seq_dev, NULL);
	dev_set_name(&seq_dev, "seq");

	if (mutex_lock_interruptible(&register_mutex))
		return -ERESTARTSYS;

	err = snd_register_device(SNDRV_DEVICE_TYPE_SEQUENCER, NULL, 0,
				  &snd_seq_f_ops, NULL, &seq_dev);
	if (err < 0) {
		mutex_unlock(&register_mutex);
		put_device(&seq_dev);
		return err;
	}
	
	mutex_unlock(&register_mutex);

	return 0;
}