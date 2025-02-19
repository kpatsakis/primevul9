static void nfs4_close_clear_stateid_flags(struct nfs4_state *state,
		fmode_t fmode)
{
	spin_lock(&state->owner->so_lock);
	if (!(fmode & FMODE_READ))
		clear_bit(NFS_O_RDONLY_STATE, &state->flags);
	if (!(fmode & FMODE_WRITE))
		clear_bit(NFS_O_WRONLY_STATE, &state->flags);
	clear_bit(NFS_O_RDWR_STATE, &state->flags);
	spin_unlock(&state->owner->so_lock);
}