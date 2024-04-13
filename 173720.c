static int __init sony_init(void)
{
	dbg_hid("Sony:%s\n", __func__);

	return hid_register_driver(&sony_driver);
}