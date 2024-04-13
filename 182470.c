SMB2_lock(const unsigned int xid, struct cifs_tcon *tcon,
	  const __u64 persist_fid, const __u64 volatile_fid, const __u32 pid,
	  const __u64 length, const __u64 offset, const __u32 lock_flags,
	  const bool wait)
{
	struct smb2_lock_element lock;

	lock.Offset = cpu_to_le64(offset);
	lock.Length = cpu_to_le64(length);
	lock.Flags = cpu_to_le32(lock_flags);
	if (!wait && lock_flags != SMB2_LOCKFLAG_UNLOCK)
		lock.Flags |= cpu_to_le32(SMB2_LOCKFLAG_FAIL_IMMEDIATELY);

	return smb2_lockv(xid, tcon, persist_fid, volatile_fid, pid, 1, &lock);
}