nwfilterStateInitialize(bool privileged,
                        const char *root,
                        virStateInhibitCallback callback G_GNUC_UNUSED,
                        void *opaque G_GNUC_UNUSED)
{
    GDBusConnection *sysbus = NULL;

    if (root != NULL) {
        virReportError(VIR_ERR_INVALID_ARG, "%s",
                       _("Driver does not support embedded mode"));
        return -1;
    }

    if (virGDBusHasSystemBus() &&
        !(sysbus = virGDBusGetSystemBus()))
        return VIR_DRV_STATE_INIT_ERROR;

    driver = g_new0(virNWFilterDriverState, 1);

    driver->lockFD = -1;
    if (virMutexInit(&driver->lock) < 0)
        goto err_free_driverstate;

    driver->privileged = privileged;
    if (!(driver->nwfilters = virNWFilterObjListNew()))
        goto error;

    if (!(driver->bindings = virNWFilterBindingObjListNew()))
        goto error;

    if (!privileged)
        return VIR_DRV_STATE_INIT_SKIPPED;

    nwfilterDriverLock();

    driver->stateDir = g_strdup(RUNSTATEDIR "/libvirt/nwfilter");

    if (g_mkdir_with_parents(driver->stateDir, S_IRWXU) < 0) {
        virReportSystemError(errno, _("cannot create state directory '%s'"),
                             driver->stateDir);
        goto error;
    }

    if ((driver->lockFD =
         virPidFileAcquire(driver->stateDir, "driver", false, getpid())) < 0)
        goto error;

    if (virNWFilterIPAddrMapInit() < 0)
        goto err_free_driverstate;
    if (virNWFilterLearnInit() < 0)
        goto err_exit_ipaddrmapshutdown;
    if (virNWFilterDHCPSnoopInit() < 0)
        goto err_exit_learnshutdown;

    if (virNWFilterTechDriversInit(privileged) < 0)
        goto err_dhcpsnoop_shutdown;

    if (virNWFilterConfLayerInit(virNWFilterTriggerRebuildImpl,
                                 driver) < 0)
        goto err_techdrivers_shutdown;

    /*
     * startup the DBus late so we don't get a reload signal while
     * initializing
     */
    if (sysbus)
        nwfilterDriverInstallDBusMatches(sysbus);

    driver->configDir = g_strdup(SYSCONFDIR "/libvirt/nwfilter");

    if (g_mkdir_with_parents(driver->configDir, S_IRWXU) < 0) {
        virReportSystemError(errno, _("cannot create config directory '%s'"),
                             driver->configDir);
        goto error;
    }

    driver->bindingDir = g_strdup(RUNSTATEDIR "/libvirt/nwfilter-binding");

    if (g_mkdir_with_parents(driver->bindingDir, S_IRWXU) < 0) {
        virReportSystemError(errno, _("cannot create config directory '%s'"),
                             driver->bindingDir);
        goto error;
    }

    if (virNWFilterObjListLoadAllConfigs(driver->nwfilters, driver->configDir) < 0)
        goto error;

    if (virNWFilterBindingObjListLoadAllConfigs(driver->bindings, driver->bindingDir) < 0)
        goto error;

    if (virNWFilterBuildAll(driver, false) < 0)
        goto error;

    nwfilterDriverUnlock();

    return VIR_DRV_STATE_INIT_COMPLETE;

 error:
    nwfilterDriverUnlock();
    nwfilterStateCleanup();

    return VIR_DRV_STATE_INIT_ERROR;

 err_techdrivers_shutdown:
    virNWFilterTechDriversShutdown();
 err_dhcpsnoop_shutdown:
    virNWFilterDHCPSnoopShutdown();
 err_exit_learnshutdown:
    virNWFilterLearnShutdown();
 err_exit_ipaddrmapshutdown:
    virNWFilterIPAddrMapShutdown();

 err_free_driverstate:
    virNWFilterObjListFree(driver->nwfilters);
    g_clear_pointer(&driver, g_free);

    return VIR_DRV_STATE_INIT_ERROR;
}