static void __exit sony_exit(void)
{
	dbg_hid("Sony:%s\n", __func__);

	hid_unregister_driver(&sony_driver);
	ida_destroy(&sony_device_id_allocator);
}