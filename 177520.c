free_footnote_ref(struct footnote_ref *ref)
{
	bufrelease(ref->contents);
	free(ref);
}