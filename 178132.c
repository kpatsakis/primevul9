static bool contains_reencryption_helper(char **names)
{
	while (*names) {
		if (is_reencryption_helper(*names++))
			return true;
	}

	return false;
}