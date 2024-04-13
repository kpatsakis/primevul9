static int __init mdesc_misc_init(void)
{
	return misc_register(&mdesc_misc);
}