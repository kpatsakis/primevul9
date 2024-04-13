Error Box::read_children(BitstreamRange& range, int max_number)
{
  int count = 0;

  while (!range.eof() && !range.error()) {
    std::shared_ptr<Box> box;
    Error error = Box::read(range, &box);
    if (error != Error::Ok) {
      return error;
    }

    if (m_children.size() > MAX_CHILDREN_PER_BOX) {
      std::stringstream sstr;
      sstr << "Maximum number of child boxes " << MAX_CHILDREN_PER_BOX << " exceeded.";

      // Sanity check.
      return Error(heif_error_Memory_allocation_error,
                   heif_suberror_Security_limit_exceeded,
                   sstr.str());
    }

    m_children.push_back(std::move(box));


    // count the new child and end reading new children when we reached the expected number

    count++;

    if (max_number != READ_CHILDREN_ALL &&
        count == max_number) {
      break;
    }
  }

  return range.get_error();
}