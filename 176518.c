static void i740fb_remove(struct pci_dev *dev)
{
	struct fb_info *info = pci_get_drvdata(dev);

	if (info) {
		struct i740fb_par *par = info->par;
		arch_phys_wc_del(par->wc_cookie);
		unregister_framebuffer(info);
		fb_dealloc_cmap(&info->cmap);
		if (par->ddc_registered)
			i2c_del_adapter(&par->ddc_adapter);
		pci_iounmap(dev, par->regs);
		pci_iounmap(dev, info->screen_base);
		pci_release_regions(dev);
/*		pci_disable_device(dev); */
		framebuffer_release(info);
	}
}