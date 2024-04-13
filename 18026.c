static bool share_conflict(struct share_mode_entry *entry,
			   uint32 access_mask,
			   uint32 share_access)
{
	DEBUG(10,("share_conflict: entry->access_mask = 0x%x, "
		  "entry->share_access = 0x%x, "
		  "entry->private_options = 0x%x\n",
		  (unsigned int)entry->access_mask,
		  (unsigned int)entry->share_access,
		  (unsigned int)entry->private_options));

	if (server_id_is_disconnected(&entry->pid)) {
		/*
		 * note: cleanup should have been done by
		 * delay_for_batch_oplocks()
		 */
		return false;
	}

	DEBUG(10,("share_conflict: access_mask = 0x%x, share_access = 0x%x\n",
		  (unsigned int)access_mask, (unsigned int)share_access));

	if ((entry->access_mask & (FILE_WRITE_DATA|
				   FILE_APPEND_DATA|
				   FILE_READ_DATA|
				   FILE_EXECUTE|
				   DELETE_ACCESS)) == 0) {
		DEBUG(10,("share_conflict: No conflict due to "
			  "entry->access_mask = 0x%x\n",
			  (unsigned int)entry->access_mask ));
		return False;
	}

	if ((access_mask & (FILE_WRITE_DATA|
			    FILE_APPEND_DATA|
			    FILE_READ_DATA|
			    FILE_EXECUTE|
			    DELETE_ACCESS)) == 0) {
		DEBUG(10,("share_conflict: No conflict due to "
			  "access_mask = 0x%x\n",
			  (unsigned int)access_mask ));
		return False;
	}

#if 1 /* JRA TEST - Superdebug. */
#define CHECK_MASK(num, am, right, sa, share) \
	DEBUG(10,("share_conflict: [%d] am (0x%x) & right (0x%x) = 0x%x\n", \
		(unsigned int)(num), (unsigned int)(am), \
		(unsigned int)(right), (unsigned int)(am)&(right) )); \
	DEBUG(10,("share_conflict: [%d] sa (0x%x) & share (0x%x) = 0x%x\n", \
		(unsigned int)(num), (unsigned int)(sa), \
		(unsigned int)(share), (unsigned int)(sa)&(share) )); \
	if (((am) & (right)) && !((sa) & (share))) { \
		DEBUG(10,("share_conflict: check %d conflict am = 0x%x, right = 0x%x, \
sa = 0x%x, share = 0x%x\n", (num), (unsigned int)(am), (unsigned int)(right), (unsigned int)(sa), \
			(unsigned int)(share) )); \
		return True; \
	}
#else
#define CHECK_MASK(num, am, right, sa, share) \
	if (((am) & (right)) && !((sa) & (share))) { \
		DEBUG(10,("share_conflict: check %d conflict am = 0x%x, right = 0x%x, \
sa = 0x%x, share = 0x%x\n", (num), (unsigned int)(am), (unsigned int)(right), (unsigned int)(sa), \
			(unsigned int)(share) )); \
		return True; \
	}
#endif

	CHECK_MASK(1, entry->access_mask, FILE_WRITE_DATA | FILE_APPEND_DATA,
		   share_access, FILE_SHARE_WRITE);
	CHECK_MASK(2, access_mask, FILE_WRITE_DATA | FILE_APPEND_DATA,
		   entry->share_access, FILE_SHARE_WRITE);

	CHECK_MASK(3, entry->access_mask, FILE_READ_DATA | FILE_EXECUTE,
		   share_access, FILE_SHARE_READ);
	CHECK_MASK(4, access_mask, FILE_READ_DATA | FILE_EXECUTE,
		   entry->share_access, FILE_SHARE_READ);

	CHECK_MASK(5, entry->access_mask, DELETE_ACCESS,
		   share_access, FILE_SHARE_DELETE);
	CHECK_MASK(6, access_mask, DELETE_ACCESS,
		   entry->share_access, FILE_SHARE_DELETE);

	DEBUG(10,("share_conflict: No conflict.\n"));
	return False;
}