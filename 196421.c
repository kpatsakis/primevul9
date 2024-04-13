int nwfilterRegister(void)
{
    if (virRegisterConnectDriver(&nwfilterConnectDriver, false) < 0)
        return -1;
    if (virSetSharedNWFilterDriver(&nwfilterDriver) < 0)
        return -1;
    if (virRegisterStateDriver(&stateDriver) < 0)
        return -1;
    return 0;
}