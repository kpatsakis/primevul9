char *restricted_pointer(char *buf, char *end, const void *ptr,
			 struct printf_spec spec)
{
	switch (kptr_restrict) {
	case 0:
		/* Always print %pK values */
		break;
	case 1: {
		const struct cred *cred;

		/*
		 * kptr_restrict==1 cannot be used in IRQ context
		 * because its test for CAP_SYSLOG would be meaningless.
		 */
		if (in_irq() || in_serving_softirq() || in_nmi()) {
			if (spec.field_width == -1)
				spec.field_width = 2 * sizeof(ptr);
			return string(buf, end, "pK-error", spec);
		}

		/*
		 * Only print the real pointer value if the current
		 * process has CAP_SYSLOG and is running with the
		 * same credentials it started with. This is because
		 * access to files is checked at open() time, but %pK
		 * checks permission at read() time. We don't want to
		 * leak pointer values if a binary opens a file using
		 * %pK and then elevates privileges before reading it.
		 */
		cred = current_cred();
		if (!has_capability_noaudit(current, CAP_SYSLOG) ||
		    !uid_eq(cred->euid, cred->uid) ||
		    !gid_eq(cred->egid, cred->gid))
			ptr = NULL;
		break;
	}
	case 2:
	default:
		/* Always print 0's for %pK */
		ptr = NULL;
		break;
	}

	return pointer_string(buf, end, ptr, spec);
}