static int bond_event_changename(struct bonding *bond)
{
	bond_remove_proc_entry(bond);
	bond_create_proc_entry(bond);

	bond_debug_reregister(bond);

	return NOTIFY_DONE;
}