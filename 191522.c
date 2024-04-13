static int ldb_msg_check_element_flags(struct ldb_context *ldb,
				       const struct ldb_message *message)
{
	unsigned i;
	for (i=0; i<message->num_elements; i++) {
		if (message->elements[i].flags & LDB_FLAG_INTERNAL_MASK) {
			ldb_asprintf_errstring(ldb, "Invalid element flags 0x%08x on element %s in %s\n",
					       message->elements[i].flags, message->elements[i].name,
					       ldb_dn_get_linearized(message->dn));
			return LDB_ERR_UNSUPPORTED_CRITICAL_EXTENSION;
		}
	}
	return LDB_SUCCESS;
}