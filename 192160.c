static int mce_device_create(unsigned int cpu)
{
	struct device *dev;
	int err;
	int i, j;

	if (!mce_available(&boot_cpu_data))
		return -EIO;

	dev = per_cpu(mce_device, cpu);
	if (dev)
		return 0;

	dev = kzalloc(sizeof *dev, GFP_KERNEL);
	if (!dev)
		return -ENOMEM;
	dev->id  = cpu;
	dev->bus = &mce_subsys;
	dev->release = &mce_device_release;

	err = device_register(dev);
	if (err) {
		put_device(dev);
		return err;
	}

	for (i = 0; mce_device_attrs[i]; i++) {
		err = device_create_file(dev, mce_device_attrs[i]);
		if (err)
			goto error;
	}
	for (j = 0; j < mca_cfg.banks; j++) {
		err = device_create_file(dev, &mce_banks[j].attr);
		if (err)
			goto error2;
	}
	cpumask_set_cpu(cpu, mce_device_initialized);
	per_cpu(mce_device, cpu) = dev;

	return 0;
error2:
	while (--j >= 0)
		device_remove_file(dev, &mce_banks[j].attr);
error:
	while (--i >= 0)
		device_remove_file(dev, mce_device_attrs[i]);

	device_unregister(dev);

	return err;
}