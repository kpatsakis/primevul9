virNWFilterTriggerRebuildImpl(void *opaque)
{
    virNWFilterDriverState *nwdriver = opaque;

    return virNWFilterBuildAll(nwdriver, true);
}