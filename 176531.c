static int i740fb_probe(struct pci_dev *dev, const struct pci_device_id *ent)
{
	struct fb_info *info;
	struct i740fb_par *par;
	int ret, tmp;
	bool found = false;
	u8 *edid;

	info = framebuffer_alloc(sizeof(struct i740fb_par), &(dev->dev));
	if (!info)
		return -ENOMEM;

	par = info->par;
	mutex_init(&par->open_lock);

	info->var.activate = FB_ACTIVATE_NOW;
	info->var.bits_per_pixel = 8;
	info->fbops = &i740fb_ops;
	info->pseudo_palette = par->pseudo_palette;

	ret = pci_enable_device(dev);
	if (ret) {
		dev_err(info->device, "cannot enable PCI device\n");
		goto err_enable_device;
	}

	ret = pci_request_regions(dev, info->fix.id);
	if (ret) {
		dev_err(info->device, "error requesting regions\n");
		goto err_request_regions;
	}

	info->screen_base = pci_ioremap_wc_bar(dev, 0);
	if (!info->screen_base) {
		dev_err(info->device, "error remapping base\n");
		ret = -ENOMEM;
		goto err_ioremap_1;
	}

	par->regs = pci_ioremap_bar(dev, 1);
	if (!par->regs) {
		dev_err(info->device, "error remapping MMIO\n");
		ret = -ENOMEM;
		goto err_ioremap_2;
	}

	/* detect memory size */
	if ((i740inreg(par, XRX, DRAM_ROW_TYPE) & DRAM_ROW_1)
							== DRAM_ROW_1_SDRAM)
		i740outb(par, XRX, DRAM_ROW_BNDRY_1);
	else
		i740outb(par, XRX, DRAM_ROW_BNDRY_0);
	info->screen_size = i740inb(par, XRX + 1) * 1024 * 1024;
	/* detect memory type */
	tmp = i740inreg(par, XRX, DRAM_ROW_CNTL_LO);
	par->has_sgram = !((tmp & DRAM_RAS_TIMING) ||
			   (tmp & DRAM_RAS_PRECHARGE));

	fb_info(info, "Intel740 on %s, %ld KB %s\n",
		pci_name(dev), info->screen_size >> 10,
		par->has_sgram ? "SGRAM" : "SDRAM");

	info->fix = i740fb_fix;
	info->fix.mmio_start = pci_resource_start(dev, 1);
	info->fix.mmio_len = pci_resource_len(dev, 1);
	info->fix.smem_start = pci_resource_start(dev, 0);
	info->fix.smem_len = info->screen_size;
	info->flags = FBINFO_DEFAULT | FBINFO_HWACCEL_YPAN;

	if (i740fb_setup_ddc_bus(info) == 0) {
		par->ddc_registered = true;
		edid = fb_ddc_read(&par->ddc_adapter);
		if (edid) {
			fb_edid_to_monspecs(edid, &info->monspecs);
			kfree(edid);
			if (!info->monspecs.modedb)
				dev_err(info->device,
					"error getting mode database\n");
			else {
				const struct fb_videomode *m;

				fb_videomode_to_modelist(
					info->monspecs.modedb,
					info->monspecs.modedb_len,
					&info->modelist);
				m = fb_find_best_display(&info->monspecs,
							 &info->modelist);
				if (m) {
					fb_videomode_to_var(&info->var, m);
					/* fill all other info->var's fields */
					if (!i740fb_check_var(&info->var, info))
						found = true;
				}
			}
		}
	}

	if (!mode_option && !found)
		mode_option = "640x480-8@60";

	if (mode_option) {
		ret = fb_find_mode(&info->var, info, mode_option,
				   info->monspecs.modedb,
				   info->monspecs.modedb_len,
				   NULL, info->var.bits_per_pixel);
		if (!ret || ret == 4) {
			dev_err(info->device, "mode %s not found\n",
				mode_option);
			ret = -EINVAL;
		}
	}

	fb_destroy_modedb(info->monspecs.modedb);
	info->monspecs.modedb = NULL;

	/* maximize virtual vertical size for fast scrolling */
	info->var.yres_virtual = info->fix.smem_len * 8 /
			(info->var.bits_per_pixel * info->var.xres_virtual);

	if (ret == -EINVAL)
		goto err_find_mode;

	ret = fb_alloc_cmap(&info->cmap, 256, 0);
	if (ret) {
		dev_err(info->device, "cannot allocate colormap\n");
		goto err_alloc_cmap;
	}

	ret = register_framebuffer(info);
	if (ret) {
		dev_err(info->device, "error registering framebuffer\n");
		goto err_reg_framebuffer;
	}

	fb_info(info, "%s frame buffer device\n", info->fix.id);
	pci_set_drvdata(dev, info);
	if (mtrr)
		par->wc_cookie = arch_phys_wc_add(info->fix.smem_start,
						  info->fix.smem_len);
	return 0;

err_reg_framebuffer:
	fb_dealloc_cmap(&info->cmap);
err_alloc_cmap:
err_find_mode:
	if (par->ddc_registered)
		i2c_del_adapter(&par->ddc_adapter);
	pci_iounmap(dev, par->regs);
err_ioremap_2:
	pci_iounmap(dev, info->screen_base);
err_ioremap_1:
	pci_release_regions(dev);
err_request_regions:
/*	pci_disable_device(dev); */
err_enable_device:
	framebuffer_release(info);
	return ret;
}