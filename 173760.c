static int hidpp_battery_get_property(struct power_supply *psy,
				      enum power_supply_property psp,
				      union power_supply_propval *val)
{
	struct hidpp_device *hidpp = power_supply_get_drvdata(psy);
	int ret = 0;

	switch(psp) {
		case POWER_SUPPLY_PROP_STATUS:
			val->intval = hidpp->battery.status;
			break;
		case POWER_SUPPLY_PROP_CAPACITY:
			val->intval = hidpp->battery.capacity;
			break;
		case POWER_SUPPLY_PROP_CAPACITY_LEVEL:
			val->intval = hidpp->battery.level;
			break;
		case POWER_SUPPLY_PROP_SCOPE:
			val->intval = POWER_SUPPLY_SCOPE_DEVICE;
			break;
		case POWER_SUPPLY_PROP_ONLINE:
			val->intval = hidpp->battery.online;
			break;
		case POWER_SUPPLY_PROP_MODEL_NAME:
			if (!strncmp(hidpp->name, "Logitech ", 9))
				val->strval = hidpp->name + 9;
			else
				val->strval = hidpp->name;
			break;
		case POWER_SUPPLY_PROP_MANUFACTURER:
			val->strval = "Logitech";
			break;
		case POWER_SUPPLY_PROP_SERIAL_NUMBER:
			val->strval = hidpp->hid_dev->uniq;
			break;
		default:
			ret = -EINVAL;
			break;
	}

	return ret;
}