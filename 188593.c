void assoc_array_cancel_edit(struct assoc_array_edit *edit)
{
	struct assoc_array_ptr *ptr;
	int i;

	pr_devel("-->%s()\n", __func__);

	/* Clean up after an out of memory error */
	for (i = 0; i < ARRAY_SIZE(edit->new_meta); i++) {
		ptr = edit->new_meta[i];
		if (ptr) {
			if (assoc_array_ptr_is_node(ptr))
				kfree(assoc_array_ptr_to_node(ptr));
			else
				kfree(assoc_array_ptr_to_shortcut(ptr));
		}
	}
	kfree(edit);
}