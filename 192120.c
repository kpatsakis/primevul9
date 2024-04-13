static inline struct mce_bank *attr_to_bank(struct device_attribute *attr)
{
	return container_of(attr, struct mce_bank, attr);
}