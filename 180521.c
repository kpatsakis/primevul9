static void __init init_iommu_pm_ops(void)
{
	register_syscore_ops(&iommu_syscore_ops);
}