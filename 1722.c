Status OutputSparse(const BatchedMap<T>& per_batch_counts, int num_values,
                    bool is_1d, OpKernelContext* context) {
  int total_values = 0;
  int num_batches = per_batch_counts.size();
  for (const auto& per_batch_count : per_batch_counts) {
    total_values += per_batch_count.size();
  }

  Tensor* indices;
  int inner_dim = is_1d ? 1 : 2;
  TF_RETURN_IF_ERROR(context->allocate_output(
      0, TensorShape({total_values, inner_dim}), &indices));

  Tensor* values;
  TF_RETURN_IF_ERROR(
      context->allocate_output(1, TensorShape({total_values}), &values));

  auto output_indices = indices->matrix<int64_t>();
  auto output_values = values->flat<T>();
  int64_t value_loc = 0;
  for (int b = 0; b < num_batches; ++b) {
    const auto& per_batch_count = per_batch_counts[b];
    std::vector<std::pair<int, T>> pairs(per_batch_count.begin(),
                                         per_batch_count.end());
    std::sort(pairs.begin(), pairs.end());
    for (const auto& x : pairs) {
      if (is_1d) {
        output_indices(value_loc, 0) = x.first;
      } else {
        output_indices(value_loc, 0) = b;
        output_indices(value_loc, 1) = x.first;
      }
      output_values(value_loc) = x.second;
      ++value_loc;
    }
  }
  Tensor* dense_shape;
  if (is_1d) {
    TF_RETURN_IF_ERROR(
        context->allocate_output(2, TensorShape({1}), &dense_shape));
    dense_shape->flat<int64_t>().data()[0] = num_values;
  } else {
    TF_RETURN_IF_ERROR(
        context->allocate_output(2, TensorShape({2}), &dense_shape));
    dense_shape->flat<int64_t>().data()[0] = num_batches;
    dense_shape->flat<int64_t>().data()[1] = num_values;
  }

  return Status::OK();
}