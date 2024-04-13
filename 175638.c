static int intel_vgpu_get_irq_count(struct intel_vgpu *vgpu, int type)
{
	if (type == VFIO_PCI_INTX_IRQ_INDEX || type == VFIO_PCI_MSI_IRQ_INDEX)
		return 1;

	return 0;
}