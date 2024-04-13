nwfilterStateCleanup(void)
{
    if (!driver)
        return -1;

    if (driver->privileged) {
        virNWFilterConfLayerShutdown();
        virNWFilterDHCPSnoopShutdown();
        virNWFilterLearnShutdown();
        virNWFilterIPAddrMapShutdown();
        virNWFilterTechDriversShutdown();

        nwfilterDriverLock();

        nwfilterDriverRemoveDBusMatches();

        if (driver->lockFD != -1)
            virPidFileRelease(driver->stateDir, "driver", driver->lockFD);

        g_free(driver->stateDir);
        g_free(driver->configDir);
        g_free(driver->bindingDir);
        nwfilterDriverUnlock();
    }

    virObjectUnref(driver->bindings);

    /* free inactive nwfilters */
    virNWFilterObjListFree(driver->nwfilters);

    virMutexDestroy(&driver->lock);
    g_clear_pointer(&driver, g_free);

    return 0;
}