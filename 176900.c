static int __init init_vdso(void)
{
	init_vdso_image(&vdso_image_64);

#ifdef CONFIG_X86_X32_ABI
	init_vdso_image(&vdso_image_x32);
#endif

	cpu_notifier_register_begin();

	on_each_cpu(vgetcpu_cpu_init, NULL, 1);
	/* notifier priority > KVM */
	__hotcpu_notifier(vgetcpu_cpu_notifier, 30);

	cpu_notifier_register_done();

	return 0;
}