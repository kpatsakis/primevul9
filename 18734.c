xemaclite_poll_controller(struct net_device *ndev)
{
	disable_irq(ndev->irq);
	xemaclite_interrupt(ndev->irq, ndev);
	enable_irq(ndev->irq);
}