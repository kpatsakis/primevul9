ddxUseMsg(void)
{
    ErrorF("\n");
    ErrorF("\n");
    ErrorF("Device Dependent Usage\n");
    if (!xf86PrivsElevated()) {
        ErrorF("-modulepath paths      specify the module search path\n");
        ErrorF("-logfile file          specify a log file name\n");
        ErrorF("-configure             probe for devices and write an "
               __XCONFIGFILE__ "\n");
        ErrorF
            ("-showopts              print available options for all installed drivers\n");
    }
    ErrorF
        ("-config file           specify a configuration file, relative to the\n");
    ErrorF("                       " __XCONFIGFILE__
           " search path, only root can use absolute\n");
    ErrorF
        ("-configdir dir         specify a configuration directory, relative to the\n");
    ErrorF("                       " __XCONFIGDIR__
           " search path, only root can use absolute\n");
    ErrorF("-verbose [n]           verbose startup messages\n");
    ErrorF("-logverbose [n]        verbose log messages\n");
    ErrorF("-quiet                 minimal startup messages\n");
    ErrorF("-pixmap24              use 24bpp pixmaps for depth 24\n");
    ErrorF("-pixmap32              use 32bpp pixmaps for depth 24\n");
    ErrorF("-fbbpp n               set bpp for the framebuffer. Default: 8\n");
    ErrorF("-depth n               set colour depth. Default: 8\n");
    ErrorF
        ("-gamma f               set gamma value (0.1 < f < 10.0) Default: 1.0\n");
    ErrorF("-rgamma f              set gamma value for red phase\n");
    ErrorF("-ggamma f              set gamma value for green phase\n");
    ErrorF("-bgamma f              set gamma value for blue phase\n");
    ErrorF
        ("-weight nnn            set RGB weighting at 16 bpp.  Default: 565\n");
    ErrorF("-layout name           specify the ServerLayout section name\n");
    ErrorF("-screen name           specify the Screen section name\n");
    ErrorF
        ("-keyboard name         specify the core keyboard InputDevice name\n");
    ErrorF
        ("-pointer name          specify the core pointer InputDevice name\n");
    ErrorF("-nosilk                disable Silken Mouse\n");
    ErrorF("-flipPixels            swap default black/white Pixel values\n");
#ifdef XF86VIDMODE
    ErrorF("-disableVidMode        disable mode adjustments with xvidtune\n");
    ErrorF
        ("-allowNonLocalXvidtune allow xvidtune to be run as a non-local client\n");
#endif
    ErrorF
        ("-allowMouseOpenFail    start server even if the mouse can't be initialized\n");
    ErrorF("-ignoreABI             make module ABI mismatches non-fatal\n");
#ifdef XSERVER_LIBPCIACCESS
    ErrorF
        ("-isolateDevice bus_id  restrict device resets to bus_id (PCI only)\n");
#endif
    ErrorF("-version               show the server version\n");
    ErrorF("-showDefaultModulePath show the server default module path\n");
    ErrorF("-showDefaultLibPath    show the server default library path\n");
    ErrorF
        ("-novtswitch            don't automatically switch VT at reset & exit\n");
    ErrorF("-sharevts              share VTs with another X server\n");
    /* OS-specific usage */
    xf86UseMsg();
    ErrorF("\n");
}