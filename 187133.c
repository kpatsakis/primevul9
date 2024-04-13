static void nfs4_destroy_slot_tables(struct nfs4_session *session)
{
	if (session->fc_slot_table.slots != NULL) {
		kfree(session->fc_slot_table.slots);
		session->fc_slot_table.slots = NULL;
	}
	if (session->bc_slot_table.slots != NULL) {
		kfree(session->bc_slot_table.slots);
		session->bc_slot_table.slots = NULL;
	}
	return;
}