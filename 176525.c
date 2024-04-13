static int i740fb_setcolreg(unsigned regno, unsigned red, unsigned green,
			   unsigned blue, unsigned transp,
			   struct fb_info *info)
{
	u32 r, g, b;

	dev_dbg(info->device, "setcolreg: regno: %i, red=%d, green=%d, blue=%d, transp=%d, bpp=%d\n",
		regno, red, green, blue, transp, info->var.bits_per_pixel);

	switch (info->fix.visual) {
	case FB_VISUAL_PSEUDOCOLOR:
		if (regno >= 256)
			return -EINVAL;
		i740outb(info->par, VGA_PEL_IW, regno);
		i740outb(info->par, VGA_PEL_D, red >> 8);
		i740outb(info->par, VGA_PEL_D, green >> 8);
		i740outb(info->par, VGA_PEL_D, blue >> 8);
		break;
	case FB_VISUAL_TRUECOLOR:
		if (regno >= 16)
			return -EINVAL;
		r = (red >> (16 - info->var.red.length))
			<< info->var.red.offset;
		b = (blue >> (16 - info->var.blue.length))
			<< info->var.blue.offset;
		g = (green >> (16 - info->var.green.length))
			<< info->var.green.offset;
		((u32 *) info->pseudo_palette)[regno] = r | g | b;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}