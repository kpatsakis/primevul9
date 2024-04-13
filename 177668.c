    int readData(
              Exiv2::IptcData& iptcData,
              uint16_t         dataSet,
              uint16_t         record,
        const Exiv2::byte*     data,
              uint32_t         sizeData
    )
    {
        Exiv2::Value::AutoPtr value;
        Exiv2::TypeId type = Exiv2::IptcDataSets::dataSetType(dataSet, record);
        value = Exiv2::Value::create(type);
        int rc = value->read(data, sizeData, Exiv2::bigEndian);
        if (0 == rc) {
            Exiv2::IptcKey key(dataSet, record);
            iptcData.add(key, value.get());
        }
        else if (1 == rc) {
            // If the first attempt failed, try with a string value
            value = Exiv2::Value::create(Exiv2::string);
            rc = value->read(data, sizeData, Exiv2::bigEndian);
            if (0 == rc) {
                Exiv2::IptcKey key(dataSet, record);
                iptcData.add(key, value.get());
            }
        }
        return rc;
    }