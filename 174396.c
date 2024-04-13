rdpsnd_register_drivers(char *options)
{
	struct audio_driver **reg;

	/* The order of registrations define the probe-order
	   when opening the device for the first time */
	reg = &drivers;
#if defined(RDPSND_ALSA)
	*reg = alsa_register(options);
	assert(*reg);
	reg = &((*reg)->next);
#endif
#if defined(RDPSND_SUN)
	*reg = sun_register(options);
	assert(*reg);
	reg = &((*reg)->next);
#endif
#if defined(RDPSND_OSS)
	*reg = oss_register(options);
	assert(*reg);
	reg = &((*reg)->next);
#endif
#if defined(RDPSND_SGI)
	*reg = sgi_register(options);
	assert(*reg);
	reg = &((*reg)->next);
#endif
#if defined(RDPSND_LIBAO)
	*reg = libao_register(options);
	assert(*reg);
	reg = &((*reg)->next);
#endif
	*reg = NULL;
}