void ExprMatchExpression::serialize(BSONObjBuilder* out) const {
    *out << "$expr" << _expression->serialize(false);
}