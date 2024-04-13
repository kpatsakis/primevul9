static int __init alsa_timer_init(void)
{
	int err;

	snd_device_initialize(&timer_dev, NULL);
	dev_set_name(&timer_dev, "timer");

#ifdef SNDRV_OSS_INFO_DEV_TIMERS
	snd_oss_info_register(SNDRV_OSS_INFO_DEV_TIMERS, SNDRV_CARDS - 1,
			      "system timer");
#endif

	err = snd_timer_register_system();
	if (err < 0) {
		pr_err("ALSA: unable to register system timer (%i)\n", err);
		goto put_timer;
	}

	err = snd_register_device(SNDRV_DEVICE_TYPE_TIMER, NULL, 0,
				  &snd_timer_f_ops, NULL, &timer_dev);
	if (err < 0) {
		pr_err("ALSA: unable to register timer device (%i)\n", err);
		snd_timer_free_all();
		goto put_timer;
	}

	snd_timer_proc_init();
	return 0;

put_timer:
	put_device(&timer_dev);
	return err;
}