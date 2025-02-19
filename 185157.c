DEFINE_PROTO_FUZZER(const test::extensions::filters::common::expr::EvaluatorTestCase& input) {
  // Create builder without constant folding.
  static Expr::BuilderPtr builder = Expr::createBuilder(nullptr);
  std::unique_ptr<TestStreamInfo> stream_info;

  try {
    // Validate that the input has an expression.
    TestUtility::validate(input);
    // Create stream_info to test against, this may catch exceptions from invalid addresses.
    stream_info = Fuzz::fromStreamInfo(input.stream_info());
  } catch (const EnvoyException& e) {
    ENVOY_LOG_MISC(debug, "EnvoyException: {}", e.what());
    return;
  }

  auto request_headers = Fuzz::fromHeaders<Http::TestRequestHeaderMapImpl>(input.request_headers());
  auto response_headers =
      Fuzz::fromHeaders<Http::TestResponseHeaderMapImpl>(input.response_headers());
  auto response_trailers = Fuzz::fromHeaders<Http::TestResponseTrailerMapImpl>(input.trailers());

  try {
    // Create the CEL expression.
    Expr::ExpressionPtr expr = Expr::createExpression(*builder, input.expression());

    // Evaluate the CEL expression.
    Protobuf::Arena arena;
    Expr::evaluate(*expr, arena, *stream_info, &request_headers, &response_headers,
                   &response_trailers);
  } catch (const CelException& e) {
    ENVOY_LOG_MISC(debug, "CelException: {}", e.what());
  }
}