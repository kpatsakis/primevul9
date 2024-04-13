Error Box_iloc::read_data(const Item& item,
                          std::shared_ptr<StreamReader> istr,
                          const std::shared_ptr<Box_idat>& idat,
                          std::vector<uint8_t>* dest) const
{
  //istr.clear();

  for (const auto& extent : item.extents) {
    if (item.construction_method == 0) {

      // --- security check that we do not allocate too much memory

      size_t old_size = dest->size();
      if (MAX_MEMORY_BLOCK_SIZE - old_size < extent.length) {
        std::stringstream sstr;
        sstr << "iloc box contained " << extent.length << " bytes, total memory size would be "
             << (old_size + extent.length) << " bytes, exceeding the security limit of "
             << MAX_MEMORY_BLOCK_SIZE << " bytes";

        return Error(heif_error_Memory_allocation_error,
                     heif_suberror_Security_limit_exceeded,
                     sstr.str());
      }


      // --- make sure that all data is available

      if (extent.offset > MAX_FILE_POS ||
          item.base_offset > MAX_FILE_POS ||
          extent.length > MAX_FILE_POS) {
        return Error(heif_error_Invalid_input,
                     heif_suberror_Security_limit_exceeded,
                     "iloc data pointers out of allowed range");
      }

      StreamReader::grow_status status = istr->wait_for_file_size(extent.offset + item.base_offset + extent.length);
      if (status == StreamReader::size_beyond_eof) {
        // Out-of-bounds
        // TODO: I think we should not clear this. Maybe we want to try reading again later and
        // hence should not lose the data already read.
        dest->clear();

        std::stringstream sstr;
        sstr << "Extent in iloc box references data outside of file bounds "
             << "(points to file position " << extent.offset + item.base_offset << ")\n";

        return Error(heif_error_Invalid_input,
                     heif_suberror_End_of_data,
                     sstr.str());
      }
      else if (status == StreamReader::timeout) {
        // TODO: maybe we should introduce some 'Recoverable error' instead of 'Invalid input'
        return Error(heif_error_Invalid_input,
                     heif_suberror_End_of_data);
      }

      // --- move file pointer to start of data

      bool success = istr->seek(extent.offset + item.base_offset);
      assert(success);
      (void)success;


      // --- read data

      dest->resize(static_cast<size_t>(old_size + extent.length));
      success = istr->read((char*)dest->data() + old_size, static_cast<size_t>(extent.length));
      assert(success);
    }
    else if (item.construction_method==1) {
      if (!idat) {
        return Error(heif_error_Invalid_input,
                     heif_suberror_No_idat_box,
                     "idat box referenced in iref box is not present in file");
      }

      idat->read_data(istr,
                      extent.offset + item.base_offset,
                      extent.length,
                      *dest);
    }
    else {
      std::stringstream sstr;
      sstr << "Item construction method " << item.construction_method << " not implemented";
      return Error(heif_error_Unsupported_feature,
                   heif_suberror_No_idat_box,
                   sstr.str());
    }
  }

  return Error::Ok;
}