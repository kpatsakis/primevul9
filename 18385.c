void add_bootloader_randomness(const void *buf, unsigned int size)
{
	if (IS_ENABLED(CONFIG_RANDOM_TRUST_BOOTLOADER))
		add_hwgenerator_randomness(buf, size, size * 8);
	else
		add_device_randomness(buf, size);
}