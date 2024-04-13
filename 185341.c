    absl::optional<absl::string_view> result() const {
      // This is safe for move/copy of this class as the backing string will be moved or copied.
      // Otherwise result_ is valid. The assert verifies that both are empty or only 1 is set.
      ASSERT((!result_.has_value() && result_backing_string_.empty()) ||
             (result_.has_value() ^ !result_backing_string_.empty()));
      return !result_backing_string_.empty() ? result_backing_string_ : result_;
    }