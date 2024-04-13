static void wcd9335_codec_remove(struct snd_soc_component *comp)
{
	struct wcd9335_codec *wcd = dev_get_drvdata(comp->dev);

	wcd_clsh_ctrl_free(wcd->clsh_ctrl);
	free_irq(regmap_irq_get_virq(wcd->irq_data, WCD9335_IRQ_SLIMBUS), wcd);
}