char *csr_chipvertostr(uint16_t ver, uint16_t rev)
{
	switch (ver) {
	case 0x00:
		return "BlueCore01a";
	case 0x01:
		switch (rev) {
		case 0x64:
			return "BlueCore01b (ES)";
		case 0x65:
		default:
			return "BlueCore01b";
		}
	case 0x02:
		switch (rev) {
		case 0x89:
			return "BlueCore02-External (ES2)";
		case 0x8a:
			return "BlueCore02-External";
		case 0x28:
			return "BlueCore02-ROM/Audio/Flash";
		default:
			return "BlueCore02";
		}
	case 0x03:
		switch (rev) {
		case 0x43:
			return "BlueCore3-MM";
		case 0x15:
			return "BlueCore3-ROM";
		case 0xe2:
			return "BlueCore3-Flash";
		case 0x26:
			return "BlueCore4-External";
		case 0x30:
			return "BlueCore4-ROM";
		default:
			return "BlueCore3 or BlueCore4";
		}
	default:
		return "Unknown";
	}
}