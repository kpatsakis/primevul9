nwfilterStateReload(void)
{
    if (!driver)
        return -1;

    if (!driver->privileged)
        return 0;

    virNWFilterDHCPSnoopEnd(NULL);
    /* shut down all threads -- they will be restarted if necessary */
    virNWFilterLearnThreadsTerminate(true);

    nwfilterDriverLock();
    virNWFilterWriteLockFilterUpdates();

    virNWFilterObjListLoadAllConfigs(driver->nwfilters, driver->configDir);

    virNWFilterUnlockFilterUpdates();

    virNWFilterBuildAll(driver, false);

    nwfilterDriverUnlock();

    return 0;
}