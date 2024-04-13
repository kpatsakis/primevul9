isdn_lock_driver(isdn_driver_t *drv)
{
	try_module_get(drv->interface->owner);
	drv->locks++;
}