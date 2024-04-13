static int tw5864_disable_input(struct tw5864_input *input)
{
	struct tw5864_dev *dev = input->root;
	unsigned long flags;

	dev_dbg(&dev->pci->dev, "Disabling channel %d\n", input->nr);

	spin_lock_irqsave(&dev->slock, flags);
	input->enabled = 0;
	spin_unlock_irqrestore(&dev->slock, flags);
	return 0;
}