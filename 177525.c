xf86PrivsElevated(void)
{
    static Bool privsTested = FALSE;
    static Bool privsElevated = TRUE;

    if (!privsTested) {
#if defined(WIN32)
        privsElevated = FALSE;
#else
        if ((getuid() != geteuid()) || (getgid() != getegid())) {
            privsElevated = TRUE;
        }
        else {
#if defined(HAVE_ISSETUGID)
            privsElevated = issetugid();
#elif defined(HAVE_GETRESUID)
            uid_t ruid, euid, suid;
            gid_t rgid, egid, sgid;

            if ((getresuid(&ruid, &euid, &suid) == 0) &&
                (getresgid(&rgid, &egid, &sgid) == 0)) {
                privsElevated = (euid != suid) || (egid != sgid);
            }
            else {
                printf("Failed getresuid or getresgid");
                /* Something went wrong, make defensive assumption */
                privsElevated = TRUE;
            }
#else
            if (getuid() == 0) {
                /* running as root: uid==euid==0 */
                privsElevated = FALSE;
            }
            else {
                /*
                 * If there are saved ID's the process might still be privileged
                 * even though the above test succeeded. If issetugid() and
                 * getresgid() aren't available, test this by trying to set
                 * euid to 0.
                 */
                unsigned int oldeuid;

                oldeuid = geteuid();

                if (seteuid(0) != 0) {
                    privsElevated = FALSE;
                }
                else {
                    if (seteuid(oldeuid) != 0) {
                        FatalError("Failed to drop privileges.  Exiting\n");
                    }
                    privsElevated = TRUE;
                }
            }
#endif
        }
#endif
        privsTested = TRUE;
    }
    return privsElevated;
}