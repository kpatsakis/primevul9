xf86CheckPrivs(const char *option, const char *arg)
{
    if (PrivsElevated() && !xf86PathIsSafe(arg)) {
        FatalError("\nInvalid argument for %s - \"%s\"\n"
                    "\tWith elevated privileges %s must specify a relative path\n"
                    "\twithout any \"..\" elements.\n\n", option, arg, option);
    }
}