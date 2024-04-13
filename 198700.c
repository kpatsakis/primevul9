static int tw5864_querycap(struct file *file, void *priv,
			   struct v4l2_capability *cap)
{
	struct tw5864_input *input = video_drvdata(file);

	strscpy(cap->driver, "tw5864", sizeof(cap->driver));
	snprintf(cap->card, sizeof(cap->card), "TW5864 Encoder %d",
		 input->nr);
	sprintf(cap->bus_info, "PCI:%s", pci_name(input->root->pci));
	return 0;
}