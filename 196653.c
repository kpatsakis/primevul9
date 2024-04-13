stack_copy (ILCodeDesc *to, ILCodeDesc *from)
{
	to->size = from->size;
	memcpy (to->stack, from->stack, sizeof (ILStackDesc) * from->size);
}