  static bool repeatedPtrFieldEqual(const Protobuf::RepeatedPtrField<ProtoType>& lhs,
                                    const Protobuf::RepeatedPtrField<ProtoType>& rhs,
                                    bool ignore_ordering = false) {
    if (lhs.size() != rhs.size()) {
      return false;
    }

    if (!ignore_ordering) {
      for (int i = 0; i < lhs.size(); ++i) {
        if (!TestUtility::protoEqual(lhs[i], rhs[i], /*ignore_ordering=*/false)) {
          return false;
        }
      }

      return true;
    }
    using ProtoList = std::list<std::unique_ptr<const Protobuf::Message>>;
    // Iterate through using protoEqual as ignore_ordering is true, and fields
    // in the sub-protos may also be out of order.
    ProtoList lhs_list =
        RepeatedPtrUtil::convertToConstMessagePtrContainer<ProtoType, ProtoList>(lhs);
    ProtoList rhs_list =
        RepeatedPtrUtil::convertToConstMessagePtrContainer<ProtoType, ProtoList>(rhs);
    while (!lhs_list.empty()) {
      bool found = false;
      for (auto it = rhs_list.begin(); it != rhs_list.end(); ++it) {
        if (TestUtility::protoEqual(*lhs_list.front(), **it,
                                    /*ignore_ordering=*/true)) {
          lhs_list.pop_front();
          rhs_list.erase(it);
          found = true;
          break;
        }
      }
      if (!found) {
        return false;
      }
    }
    return true;
  }