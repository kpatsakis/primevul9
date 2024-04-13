static int i740fb_blank(int blank_mode, struct fb_info *info)
{
	struct i740fb_par *par = info->par;

	unsigned char SEQ01;
	int DPMSSyncSelect;

	switch (blank_mode) {
	case FB_BLANK_UNBLANK:
	case FB_BLANK_NORMAL:
		SEQ01 = 0x00;
		DPMSSyncSelect = HSYNC_ON | VSYNC_ON;
		break;
	case FB_BLANK_VSYNC_SUSPEND:
		SEQ01 = 0x20;
		DPMSSyncSelect = HSYNC_ON | VSYNC_OFF;
		break;
	case FB_BLANK_HSYNC_SUSPEND:
		SEQ01 = 0x20;
		DPMSSyncSelect = HSYNC_OFF | VSYNC_ON;
		break;
	case FB_BLANK_POWERDOWN:
		SEQ01 = 0x20;
		DPMSSyncSelect = HSYNC_OFF | VSYNC_OFF;
		break;
	default:
		return -EINVAL;
	}
	/* Turn the screen on/off */
	i740outb(par, SRX, 0x01);
	SEQ01 |= i740inb(par, SRX + 1) & ~0x20;
	i740outb(par, SRX, 0x01);
	i740outb(par, SRX + 1, SEQ01);

	/* Set the DPMS mode */
	i740outreg(par, XRX, DPMS_SYNC_SELECT, DPMSSyncSelect);

	/* Let fbcon do a soft blank for us */
	return (blank_mode == FB_BLANK_NORMAL) ? 1 : 0;
}