epass2003_hook_path(struct sc_path *path, int inc)
{
	u8 fid_h = path->value[path->len - 2];
	u8 fid_l = path->value[path->len - 1];

	switch (fid_h) {
	case 0x29:
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
	case 0x34:
		if (inc)
			fid_l = fid_l * FID_STEP;
		else
			fid_l = fid_l / FID_STEP;
		path->value[path->len - 1] = fid_l;
		return 1;
	default:
		break;
	}
	return 0;
}