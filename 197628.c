static inline int pufd_omsg_get_channel(struct pcan_ufd_ovr_msg *om)
{
	return om->channel & 0xf;
}