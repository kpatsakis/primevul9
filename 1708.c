Status InitConv2DParameters(const OpKernelConstruction* context,
                            Conv2DParameters* params) {
  TF_RETURN_IF_ERROR(context->GetAttr("dilations", &params->dilations));
  TF_RETURN_IF_ERROR(context->GetAttr("strides", &params->strides));
  TF_RETURN_IF_ERROR(context->GetAttr("padding", &params->padding));
  if (context->HasAttr("explicit_paddings")) {
    TF_RETURN_IF_ERROR(
        context->GetAttr("explicit_paddings", &params->explicit_paddings));
  }
  string data_format_string;
  TF_RETURN_IF_ERROR(context->GetAttr("data_format", &data_format_string));
  TF_REQUIRES(FormatFromString(data_format_string, &params->data_format),
              errors::InvalidArgument("Invalid data format"));

  const auto& strides = params->strides;
  const auto& dilations = params->dilations;
  const auto& data_format = params->data_format;

  TF_REQUIRES(dilations.size() == 4,
              errors::InvalidArgument("Sliding window dilations field must "
                                      "specify 4 dimensions"));
  TF_REQUIRES(strides.size() == 4,
              errors::InvalidArgument("Sliding window strides field must "
                                      "specify 4 dimensions"));
  const int64_t stride_n = GetTensorDim(strides, data_format, 'N');
  const int64_t stride_c = GetTensorDim(strides, data_format, 'C');
  const int64_t stride_h = GetTensorDim(strides, data_format, 'H');
  const int64_t stride_w = GetTensorDim(strides, data_format, 'W');
  TF_REQUIRES(
      stride_n == 1 && stride_c == 1,
      errors::Unimplemented("Current implementation does not yet support "
                            "strides in the batch and depth dimensions."));
  TF_REQUIRES(stride_h > 0 && stride_w > 0,
              errors::InvalidArgument(
                  "Row and column strides should be larger than 0."));

  const int64_t dilation_n = GetTensorDim(dilations, data_format, 'N');
  const int64_t dilation_c = GetTensorDim(dilations, data_format, 'C');
  const int64_t dilation_h = GetTensorDim(dilations, data_format, 'H');
  const int64_t dilation_w = GetTensorDim(dilations, data_format, 'W');
  TF_REQUIRES(
      dilation_n == 1 && dilation_c == 1,
      errors::Unimplemented("Current implementation does not yet support "
                            "dilations in the batch and depth dimensions."));
  TF_REQUIRES(
      dilation_h > 0 && dilation_w > 0,
      errors::InvalidArgument("Dilated rates should be larger than 0."));

  TF_RETURN_IF_ERROR(CheckValidPadding(params->padding,
                                       params->explicit_paddings,
                                       /*num_dims=*/4, data_format));

  return Status::OK();
}