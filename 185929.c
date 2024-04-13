    std::string PngChunk::makeMetadataChunk(const std::string& metadata,
                                                  MetadataId   type)
    {
        std::string chunk;
        std::string rawProfile;

        switch (type) {
        case mdComment:
            chunk = makeUtf8TxtChunk("Description", metadata, true);
            break;
        case mdExif:
            rawProfile = writeRawProfile(metadata, "exif");
            chunk = makeAsciiTxtChunk("Raw profile type exif", rawProfile, true);
            break;
        case mdIptc:
            rawProfile = writeRawProfile(metadata, "iptc");
            chunk = makeAsciiTxtChunk("Raw profile type iptc", rawProfile, true);
            break;
        case mdXmp:
            chunk = makeUtf8TxtChunk("XML:com.adobe.xmp", metadata, false);
            break;
        case mdIccProfile:
            break;
        case mdNone:
            assert(false);
    }

        return chunk;

    } // PngChunk::makeMetadataChunk