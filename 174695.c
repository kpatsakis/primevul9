OsVendorFatalError(const char *f, va_list args)
{
#ifdef VENDORSUPPORT
    ErrorFSigSafe("\nPlease refer to your Operating System Vendor support "
                 "pages\nat %s for support on this crash.\n", VENDORSUPPORT);
#else
    ErrorFSigSafe("\nPlease consult the " XVENDORNAME " support \n\t at "
                 __VENDORDWEBSUPPORT__ "\n for help. \n");
#endif
    if (xf86LogFile && xf86LogFileWasOpened)
        ErrorFSigSafe("Please also check the log file at \"%s\" for additional "
                     "information.\n", xf86LogFile);
    ErrorFSigSafe("\n");
}