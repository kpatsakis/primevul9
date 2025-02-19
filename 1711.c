  void operator()(OpKernelContext* ctx, bool use_cudnn, bool cudnn_use_autotune,
                  const Tensor& input, const Tensor& filter, int row_dilation,
                  int col_dilation, int row_stride, int col_stride,
                  const Padding& padding,
                  const std::vector<int64_t>& explicit_paddings, Tensor* output,
                  TensorFormat data_format) {
    if (data_format != FORMAT_NHWC) {
      ctx->SetStatus(errors::Unimplemented(
          "The Conv2D op currently only supports the NHWC tensor format on the "
          "CPU. The op was given the format: ",
          ToString(data_format)));
      return;
    }

    for (int64_t explicit_padding : explicit_paddings) {
      if (!FastBoundsCheck(explicit_padding, std::numeric_limits<int>::max())) {
        ctx->SetStatus(errors::InvalidArgument("filter too large"));
        return;
      }
    }

    const int64_t in_depth = input.dim_size(3);
    const int64_t out_depth = output->dim_size(3);
    const int64_t patch_depth = filter.dim_size(2);

    if (patch_depth <= 0) {
      ctx->SetStatus(errors::InvalidArgument(
          "filter depth must be stricly positive, got ", patch_depth));
      return;
    }
    if (in_depth % patch_depth != 0) {
      ctx->SetStatus(errors::InvalidArgument(
          "input depth must be evenly divisible by filter depth: ", in_depth,
          " vs ", patch_depth));
      return;
    }
    if (filter.NumElements() <= 0) {
      ctx->SetStatus(
          errors::InvalidArgument("filter must not have zero elements "
                                  "(i.e. all dimensions must be non-zero)"));
      return;
    }

    const int64_t num_groups = in_depth / patch_depth;
    if (num_groups <= 0) {
      ctx->SetStatus(errors::InvalidArgument(
          "number of groups must be stricly positive, got ", num_groups));
      return;
    }
    if (out_depth % num_groups != 0 || out_depth < num_groups) {
      ctx->SetStatus(errors::InvalidArgument(
          "output depth must be evenly divisible by number of groups: ",
          out_depth, " vs ", num_groups));
      return;
    }

    if (in_depth != patch_depth) {
      LaunchGrouped<T>()(ctx, input, filter, row_stride, col_stride,
                         row_dilation, col_dilation, padding, explicit_paddings,
                         output, data_format);
    } else {
      LaunchGeneric<CPUDevice, T>()(ctx, input, filter, row_stride, col_stride,
                                    row_dilation, col_dilation, padding,
                                    explicit_paddings, output, data_format);
    }
  }