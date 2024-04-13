void WriteByte(cmsIOHANDLER* m, cmsUInt8Number b)
{
    _cmsIOPrintf(m, "%02x", b);
    _cmsPSActualColumn += 2;

    if (_cmsPSActualColumn > MAXPSCOLS) {

        _cmsIOPrintf(m, "\n");
        _cmsPSActualColumn = 0;
    }
}