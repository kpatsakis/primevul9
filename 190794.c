usesLongNames (const Header &header)
{
    //
    // If an OpenEXR file contains any attribute names, attribute type names
    // or channel names longer than 31 characters, then the file cannot be
    // read by older versions of the OpenEXR library (up to OpenEXR 1.6.1).
    // Before writing the file header, we check if the header contains
    // any names longer than 31 characters; if it does, then we set the
    // LONG_NAMES_FLAG in the file version number.  Older versions of the
    // OpenEXR library will refuse to read files that have the LONG_NAMES_FLAG
    // set.  Without the flag, older versions of the library would mis-
    // interpret the file as broken.
    //

    for (Header::ConstIterator i = header.begin();
         i != header.end();
         ++i)
    {
        if (strlen (i.name()) >= 32 || strlen (i.attribute().typeName()) >= 32)
            return true;
    }

    const ChannelList &channels = header.channels();

    for (ChannelList::ConstIterator i = channels.begin();
         i != channels.end();
         ++i)
    {
        if (strlen (i.name()) >= 32)
            return true;
    }

    return false;
}