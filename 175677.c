static int kvmgt_write_gpa(unsigned long handle, unsigned long gpa,
			void *buf, unsigned long len)
{
	return kvmgt_rw_gpa(handle, gpa, buf, len, true);
}