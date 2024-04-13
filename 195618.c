static struct ext4_new_flex_group_data *alloc_flex_gd(unsigned long flexbg_size)
{
	struct ext4_new_flex_group_data *flex_gd;

	flex_gd = kmalloc(sizeof(*flex_gd), GFP_NOFS);
	if (flex_gd == NULL)
		goto out3;

	if (flexbg_size >= UINT_MAX / sizeof(struct ext4_new_flex_group_data))
		goto out2;
	flex_gd->count = flexbg_size;

	flex_gd->groups = kmalloc(sizeof(struct ext4_new_group_data) *
				  flexbg_size, GFP_NOFS);
	if (flex_gd->groups == NULL)
		goto out2;

	flex_gd->bg_flags = kmalloc(flexbg_size * sizeof(__u16), GFP_NOFS);
	if (flex_gd->bg_flags == NULL)
		goto out1;

	return flex_gd;

out1:
	kfree(flex_gd->groups);
out2:
	kfree(flex_gd);
out3:
	return NULL;
}