void putname(struct filename *name)
{
	if (unlikely(!audit_dummy_context()))
		return audit_putname(name);
	final_putname(name);
}