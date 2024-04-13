static void dwc3_pci_resume_work(struct work_struct *work)
{
	struct dwc3_pci *dwc = container_of(work, struct dwc3_pci, wakeup_work);
	struct platform_device *dwc3 = dwc->dwc3;
	int ret;

	ret = pm_runtime_get_sync(&dwc3->dev);
	if (ret)
		return;

	pm_runtime_mark_last_busy(&dwc3->dev);
	pm_runtime_put_sync_autosuspend(&dwc3->dev);
}