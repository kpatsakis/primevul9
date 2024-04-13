isdn_unlock_driver(isdn_driver_t *drv)
{
	if (drv->locks > 0) {
		drv->locks--;
		module_put(drv->interface->owner);
	}
}