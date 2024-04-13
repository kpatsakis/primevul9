static u8 coolkey_class(unsigned long object_id) {
	return (object_id >> 24) & 0xff;
}