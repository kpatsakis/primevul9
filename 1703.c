  void operator()(OpKernelContext* ctx, bool use_cudnn, bool cudnn_use_autotune,
                  const Tensor& input, const Tensor& filter, int row_dilation,
                  int col_dilation, int row_stride, int col_stride,
                  const Padding& padding,
                  const std::vector<int64_t>& explicit_paddings, Tensor* output,
                  TensorFormat data_format) {
    if (data_format != FORMAT_NHWC) {
      ctx->SetStatus(
          errors::Unimplemented("The Conv2D op currently only supports the "
                                "NHWC tensor format for integer types. "
                                "The op was given the format: ",
                                ToString(data_format)));
      return;
    }
    const int64_t in_depth = GetTensorDim(input, data_format, 'C');
    OP_REQUIRES(ctx, in_depth == filter.dim_size(2),
                errors::Unimplemented(
                    "The Conv2D op currently does not support grouped "
                    "convolutions for integer types. A grouped convolution was "
                    "attempted to be run because the input depth of ",
                    in_depth, " does not match the filter input depth of ",
                    filter.dim_size(2)));
    OP_REQUIRES(
        ctx, filter.NumElements() > 0,
        errors::InvalidArgument("filter must not have zero elements "
                                "(i.e. all dimensions must be non-zero)"));

    for (int64_t explicit_padding : explicit_paddings) {
      if (!FastBoundsCheck(explicit_padding, std::numeric_limits<int>::max())) {
        ctx->SetStatus(errors::InvalidArgument("filter too large"));
        return;
      }
    }
    LaunchGeneric<GPUDevice, int32>()(
        ctx, input, filter, row_stride, col_stride, row_dilation, col_dilation,
        padding, explicit_paddings, output, data_format);
  }