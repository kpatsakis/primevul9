static u32 ql_get_link_speed(struct ql3_adapter *qdev)
{
	if (ql_is_fiber(qdev))
		return SPEED_1000;
	else
		return ql_phy_get_speed(qdev);
}