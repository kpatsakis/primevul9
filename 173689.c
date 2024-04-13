static int sony_battery_get_property(struct power_supply *psy,
				     enum power_supply_property psp,
				     union power_supply_propval *val)
{
	struct sony_sc *sc = power_supply_get_drvdata(psy);
	unsigned long flags;
	int ret = 0;
	u8 battery_charging, battery_capacity, cable_state;

	spin_lock_irqsave(&sc->lock, flags);
	battery_charging = sc->battery_charging;
	battery_capacity = sc->battery_capacity;
	cable_state = sc->cable_state;
	spin_unlock_irqrestore(&sc->lock, flags);

	switch (psp) {
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = 1;
		break;
	case POWER_SUPPLY_PROP_SCOPE:
		val->intval = POWER_SUPPLY_SCOPE_DEVICE;
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		val->intval = battery_capacity;
		break;
	case POWER_SUPPLY_PROP_STATUS:
		if (battery_charging)
			val->intval = POWER_SUPPLY_STATUS_CHARGING;
		else
			if (battery_capacity == 100 && cable_state)
				val->intval = POWER_SUPPLY_STATUS_FULL;
			else
				val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}