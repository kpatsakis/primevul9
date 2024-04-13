static u32 bond_mode_bcast_speed(struct slave *slave, u32 speed)
{
	if (speed == 0 || speed == SPEED_UNKNOWN)
		speed = slave->speed;
	else
		speed = min(speed, slave->speed);

	return speed;
}