nwfilterObjFromNWFilter(const unsigned char *uuid)
{
    virNWFilterObj *obj;
    char uuidstr[VIR_UUID_STRING_BUFLEN];

    if (!(obj = virNWFilterObjListFindByUUID(driver->nwfilters, uuid))) {
        virUUIDFormat(uuid, uuidstr);
        virReportError(VIR_ERR_NO_NWFILTER,
                       _("no nwfilter with matching uuid '%s'"), uuidstr);
    }
    return obj;
}