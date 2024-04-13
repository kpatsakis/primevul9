QPDFObjectHandle::isPageObject()
{
    // See comments in QPDFObjectHandle.hh.
    if (! this->isDictionary())
    {
        return false;
    }
    if (this->hasKey("/Type"))
    {
        QPDFObjectHandle type = this->getKey("/Type");
        if (type.isName() && (type.getName() == "/Page"))
        {
            return true;
        }
        // Files have been seen in the wild that have /Type (Page)
        if (type.isString() && (type.getStringValue() == "Page"))
        {
            return true;
        }
    }
    if (this->hasKey("/Contents"))
    {
        return true;
    }
    return false;
}