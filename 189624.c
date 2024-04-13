static u32 wmi_addr_remap(u32 x)
{
	uint i;

	for (i = 0; i < ARRAY_SIZE(fw_mapping); i++) {
		if (fw_mapping[i].fw &&
		    ((x >= fw_mapping[i].from) && (x < fw_mapping[i].to)))
			return x + fw_mapping[i].host - fw_mapping[i].from;
	}

	return 0;
}