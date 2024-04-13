initable_iface_init (GInitableIface *initable_iface)
{
	initable_iface->init = gxps_archive_initable_init;
}