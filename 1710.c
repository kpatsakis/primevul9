  void operator()(OpKernelContext* ctx, const Tensor& input,
                  const Tensor& filter, int row_stride, int col_stride,
                  int row_dilation, int col_dilation, const Padding& padding,
                  const std::vector<int64_t>& explicit_paddings, Tensor* output,
                  TensorFormat data_format) {
    CHECK(data_format == FORMAT_NHWC) << "Generic conv implementation only "
                                         "supports NHWC tensor format for now.";
    if (filter.dim_size(0) == 1 && filter.dim_size(1) == 1 && row_stride == 1 &&
        col_stride == 1 && (padding == SAME || padding == VALID)) {
      // For 1x1 kernel, the 2D convolution is reduced to matrix
      // multiplication.
      //
      // TODO(vrv): We should be able to call SpatialConvolution
      // and it will produce the same result, but doing so
      // led to NaNs during training.  Using matmul instead for now.
      int conv_width = 1;  // Width for the convolution step.
      for (int i = 0; i < 3; ++i) {
        conv_width *= output->dim_size(i);
      }

      Eigen::array<Eigen::IndexPair<Eigen::DenseIndex>, 1> dim_pair;
      dim_pair[0] = Eigen::IndexPair<Eigen::DenseIndex>(1, 0);
      functor::MatMulConvFunctor<Device, T>()(
          ctx->eigen_device<Device>(),
          output->shaped<T, 2>({conv_width, filter.dim_size(3)}),
          input.shaped<T, 2>({conv_width, filter.dim_size(2)}),
          filter.shaped<T, 2>({filter.dim_size(2), filter.dim_size(3)}),
          dim_pair);
    } else if (filter.dim_size(0) == input.dim_size(1) &&
               filter.dim_size(1) == input.dim_size(2) && row_dilation == 1 &&
               col_dilation == 1 && padding == VALID) {
      // If the input data and filter have the same height/width,
      // the 2D convolution is reduced to matrix multiplication.
      const int k =  // Length of reduction dimension.
          filter.dim_size(0) * filter.dim_size(1) * filter.dim_size(2);

      Eigen::array<Eigen::IndexPair<Eigen::DenseIndex>, 1> dim_pair;
      dim_pair[0] = Eigen::IndexPair<Eigen::DenseIndex>(1, 0);
      functor::MatMulConvFunctor<Device, T>()(
          ctx->eigen_device<Device>(),
          output->shaped<T, 2>({input.dim_size(0), filter.dim_size(3)}),
          input.shaped<T, 2>({input.dim_size(0), k}),
          filter.shaped<T, 2>({k, filter.dim_size(3)}), dim_pair);
    } else {
      if (padding == EXPLICIT) {
        functor::SpatialConvolution<Device, T>()(
            ctx->eigen_device<Device>(), output->tensor<T, 4>(),
            input.tensor<T, 4>(), filter.tensor<T, 4>(), row_stride, col_stride,
            row_dilation, col_dilation, static_cast<int>(explicit_paddings[2]),
            static_cast<int>(explicit_paddings[3]),
            static_cast<int>(explicit_paddings[4]),
            static_cast<int>(explicit_paddings[5]));
      } else {
        functor::SpatialConvolution<Device, T>()(
            ctx->eigen_device<Device>(), output->tensor<T, 4>(),
            input.tensor<T, 4>(), filter.tensor<T, 4>(), row_stride, col_stride,
            row_dilation, col_dilation, BrainPadding2EigenPadding(padding));
      }
    }
  }