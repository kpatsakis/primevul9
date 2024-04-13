static u16 ml_calculate_direction(u16 direction, u16 force,
				  u16 new_direction, u16 new_force)
{
	if (!force)
		return new_direction;
	if (!new_force)
		return direction;
	return (((u32)(direction >> 1) * force +
		 (new_direction >> 1) * new_force) /
		(force + new_force)) << 1;
}