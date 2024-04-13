  template <typename UnaryOperation> void inlineTransform(UnaryOperation&& unary_op) {
    ASSERT(type() == Type::Inline);
    std::transform(absl::get<InlineHeaderVector>(buffer_).begin(),
                   absl::get<InlineHeaderVector>(buffer_).end(),
                   absl::get<InlineHeaderVector>(buffer_).begin(), unary_op);
  }