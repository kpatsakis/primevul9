static NORETURN void die_initial_contact(int unexpected)
{
	/*
	 * A hang-up after seeing some response from the other end
	 * means that it is unexpected, as we know the other end is
	 * willing to talk to us.  A hang-up before seeing any
	 * response does not necessarily mean an ACL problem, though.
	 */
	if (unexpected)
		die(_("the remote end hung up upon initial contact"));
	else
		die(_("Could not read from remote repository.\n\n"
		      "Please make sure you have the correct access rights\n"
		      "and the repository exists."));
}