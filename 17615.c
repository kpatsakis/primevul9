int snd_ctl_register_ioctl_compat(snd_kctl_ioctl_func_t fcn)
{
	return _snd_ctl_register_ioctl(fcn, &snd_control_compat_ioctls);
}