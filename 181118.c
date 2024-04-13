u8 ath6kl_wmi_get_traffic_class(u8 user_priority)
{
	return  up_to_ac[user_priority & 0x7];
}