    TiffComponent::AutoPtr TiffCreator::create(uint32_t extendedTag,
                                               IfdId    group)
    {
        TiffComponent::AutoPtr tc(0);
        uint16_t tag = static_cast<uint16_t>(extendedTag & 0xffff);
        const TiffGroupStruct* ts = find(tiffGroupStruct_,
                                         TiffGroupStruct::Key(extendedTag, group));
        if (ts && ts->newTiffCompFct_) {
            tc = ts->newTiffCompFct_(tag, group);
        }
#ifdef DEBUG
        else {
            if (!ts) {
                std::cerr << "Warning: No TIFF structure entry found for ";
            }
            else {
                std::cerr << "Warning: No TIFF component creator found for ";
            }
            std::cerr << "extended tag 0x" << std::setw(4) << std::setfill('0')
                      << std::hex << std::right << extendedTag
                      << ", group " << groupName(group) << "\n";
        }
#endif
        return tc;
    } // TiffCreator::create