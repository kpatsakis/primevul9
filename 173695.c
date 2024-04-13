static void hidpp_prefix_name(char **name, int name_length)
{
#define PREFIX_LENGTH 9 /* "Logitech " */

	int new_length;
	char *new_name;

	if (name_length > PREFIX_LENGTH &&
	    strncmp(*name, "Logitech ", PREFIX_LENGTH) == 0)
		/* The prefix has is already in the name */
		return;

	new_length = PREFIX_LENGTH + name_length;
	new_name = kzalloc(new_length, GFP_KERNEL);
	if (!new_name)
		return;

	snprintf(new_name, new_length, "Logitech %s", *name);

	kfree(*name);

	*name = new_name;
}