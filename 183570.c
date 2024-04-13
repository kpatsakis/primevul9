static void igmp_group_dropped(struct ip_mc_list *im)
{
	__igmp_group_dropped(im, GFP_KERNEL);
}