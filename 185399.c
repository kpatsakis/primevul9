ExpressionPtr createExpression(Builder& builder, const google::api::expr::v1alpha1::Expr& expr) {
  google::api::expr::v1alpha1::SourceInfo source_info;
  auto cel_expression_status = builder.CreateExpression(&expr, &source_info);
  if (!cel_expression_status.ok()) {
    throw CelException(
        absl::StrCat("failed to create an expression: ", cel_expression_status.status().message()));
  }
  return std::move(cel_expression_status.value());
}