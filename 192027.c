static int rtl8xxxu_gen1_channel_to_group(int channel)
{
	int group;

	if (channel < 4)
		group = 0;
	else if (channel < 10)
		group = 1;
	else
		group = 2;

	return group;
}