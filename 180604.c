static inline unsigned long intel_iommu_get_pts(struct device *dev)
{
	int pts, max_pasid;

	max_pasid = intel_pasid_get_dev_max_id(dev);
	pts = find_first_bit((unsigned long *)&max_pasid, MAX_NR_PASID_BITS);
	if (pts < 5)
		return 0;

	return pts - 5;
}