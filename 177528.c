xf86PrintBanner(void)
{
#if PRE_RELEASE
    xf86ErrorFVerb(0, "\n"
                   "This is a pre-release version of the X server from "
                   XVENDORNAME ".\n" "It is not supported in any way.\n"
                   "Bugs may be filed in the bugzilla at http://bugs.freedesktop.org/.\n"
                   "Select the \"xorg\" product for bugs you find in this release.\n"
                   "Before reporting bugs in pre-release versions please check the\n"
                   "latest version in the X.Org Foundation git repository.\n"
                   "See http://wiki.x.org/wiki/GitPage for git access instructions.\n");
#endif
    xf86ErrorFVerb(0, "\nX.Org X Server %d.%d.%d",
                   XORG_VERSION_MAJOR, XORG_VERSION_MINOR, XORG_VERSION_PATCH);
#if XORG_VERSION_SNAP > 0
    xf86ErrorFVerb(0, ".%d", XORG_VERSION_SNAP);
#endif

#if XORG_VERSION_SNAP >= 900
    /* When the minor number is 99, that signifies that the we are making
     * a release candidate for a major version.  (X.0.0)
     * When the patch number is 99, that signifies that the we are making
     * a release candidate for a minor version.  (X.Y.0)
     * When the patch number is < 99, then we are making a release
     * candidate for the next point release.  (X.Y.Z)
     */
#if XORG_VERSION_MINOR >= 99
    xf86ErrorFVerb(0, " (%d.0.0 RC %d)", XORG_VERSION_MAJOR + 1,
                   XORG_VERSION_SNAP - 900);
#elif XORG_VERSION_PATCH == 99
    xf86ErrorFVerb(0, " (%d.%d.0 RC %d)", XORG_VERSION_MAJOR,
                   XORG_VERSION_MINOR + 1, XORG_VERSION_SNAP - 900);
#else
    xf86ErrorFVerb(0, " (%d.%d.%d RC %d)", XORG_VERSION_MAJOR,
                   XORG_VERSION_MINOR, XORG_VERSION_PATCH + 1,
                   XORG_VERSION_SNAP - 900);
#endif
#endif

#ifdef XORG_CUSTOM_VERSION
    xf86ErrorFVerb(0, " (%s)", XORG_CUSTOM_VERSION);
#endif
#ifndef XORG_DATE
#define XORG_DATE "Unknown"
#endif
    xf86ErrorFVerb(0, "\nRelease Date: %s\n", XORG_DATE);
    xf86ErrorFVerb(0, "X Protocol Version %d, Revision %d\n",
                   X_PROTOCOL, X_PROTOCOL_REVISION);
    xf86ErrorFVerb(0, "Build Operating System: %s %s\n", OSNAME, OSVENDOR);
#ifdef HAS_UTSNAME
    {
        struct utsname name;

        /* Linux & BSD state that 0 is success, SysV (including Solaris, HP-UX,
           and Irix) and Single Unix Spec 3 just say that non-negative is success.
           All agree that failure is represented by a negative number.
         */
        if (uname(&name) >= 0) {
            xf86ErrorFVerb(0, "Current Operating System: %s %s %s %s %s\n",
                           name.sysname, name.nodename, name.release,
                           name.version, name.machine);
#ifdef linux
            do {
                char buf[80];
                int fd = open("/proc/cmdline", O_RDONLY);

                if (fd != -1) {
                    xf86ErrorFVerb(0, "Kernel command line: ");
                    memset(buf, 0, 80);
                    while (read(fd, buf, 80) > 0) {
                        xf86ErrorFVerb(0, "%.80s", buf);
                        memset(buf, 0, 80);
                    }
                    close(fd);
                }
            } while (0);
#endif
        }
    }
#endif
#if defined(BUILD_DATE) && (BUILD_DATE > 19000000)
    {
        struct tm t;
        char buf[100];

        memset(&t, 0, sizeof(t));
        memset(buf, 0, sizeof(buf));
        t.tm_mday = BUILD_DATE % 100;
        t.tm_mon = (BUILD_DATE / 100) % 100 - 1;
        t.tm_year = BUILD_DATE / 10000 - 1900;
#if defined(BUILD_TIME)
        t.tm_sec = BUILD_TIME % 100;
        t.tm_min = (BUILD_TIME / 100) % 100;
        t.tm_hour = (BUILD_TIME / 10000) % 100;
        if (strftime(buf, sizeof(buf), "%d %B %Y  %I:%M:%S%p", &t))
            xf86ErrorFVerb(0, "Build Date: %s\n", buf);
#else
        if (strftime(buf, sizeof(buf), "%d %B %Y", &t))
            xf86ErrorFVerb(0, "Build Date: %s\n", buf);
#endif
    }
#endif
#if defined(BUILDERSTRING)
    xf86ErrorFVerb(0, "%s \n", BUILDERSTRING);
#endif
    xf86ErrorFVerb(0, "Current version of pixman: %s\n",
                   pixman_version_string());
    xf86ErrorFVerb(0, "\tBefore reporting problems, check "
                   "" __VENDORDWEBSUPPORT__ "\n"
                   "\tto make sure that you have the latest version.\n");
}