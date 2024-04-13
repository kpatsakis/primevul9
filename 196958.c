onig_error_code_to_format(int code)
{
  char *p;

  if (code >= 0) return (UChar* )0;

  switch (code) {
  case ONIG_MISMATCH:
    p = "mismatch"; break;
  case ONIG_NO_SUPPORT_CONFIG:
    p = "no support in this configuration"; break;
  case ONIGERR_MEMORY:
    p = "fail to memory allocation"; break;
  case ONIGERR_MATCH_STACK_LIMIT_OVER:
    p = "match-stack limit over"; break;
  case ONIGERR_PARSE_DEPTH_LIMIT_OVER:
    p = "parse depth limit over"; break;
  case ONIGERR_TYPE_BUG:
    p = "undefined type (bug)"; break;
  case ONIGERR_PARSER_BUG:
    p = "internal parser error (bug)"; break;
  case ONIGERR_STACK_BUG:
    p = "stack error (bug)"; break;
  case ONIGERR_UNDEFINED_BYTECODE:
    p = "undefined bytecode (bug)"; break;
  case ONIGERR_UNEXPECTED_BYTECODE:
    p = "unexpected bytecode (bug)"; break;
  case ONIGERR_DEFAULT_ENCODING_IS_NOT_SETTED:
    p = "default multibyte-encoding is not setted"; break;
  case ONIGERR_SPECIFIED_ENCODING_CANT_CONVERT_TO_WIDE_CHAR:
    p = "can't convert to wide-char on specified multibyte-encoding"; break;
  case ONIGERR_FAIL_TO_INITIALIZE:
    p = "fail to initialize"; break;
  case ONIGERR_INVALID_ARGUMENT:
    p = "invalid argument"; break;
  case ONIGERR_END_PATTERN_AT_LEFT_BRACE:
    p = "end pattern at left brace"; break;
  case ONIGERR_END_PATTERN_AT_LEFT_BRACKET:
    p = "end pattern at left bracket"; break;
  case ONIGERR_EMPTY_CHAR_CLASS:
    p = "empty char-class"; break;
  case ONIGERR_PREMATURE_END_OF_CHAR_CLASS:
    p = "premature end of char-class"; break;
  case ONIGERR_END_PATTERN_AT_ESCAPE:
    p = "end pattern at escape"; break;
  case ONIGERR_END_PATTERN_AT_META:
    p = "end pattern at meta"; break;
  case ONIGERR_END_PATTERN_AT_CONTROL:
    p = "end pattern at control"; break;
  case ONIGERR_META_CODE_SYNTAX:
    p = "invalid meta-code syntax"; break;
  case ONIGERR_CONTROL_CODE_SYNTAX:
    p = "invalid control-code syntax"; break;
  case ONIGERR_CHAR_CLASS_VALUE_AT_END_OF_RANGE:
    p = "char-class value at end of range"; break;
  case ONIGERR_CHAR_CLASS_VALUE_AT_START_OF_RANGE:
    p = "char-class value at start of range"; break;
  case ONIGERR_UNMATCHED_RANGE_SPECIFIER_IN_CHAR_CLASS:
    p = "unmatched range specifier in char-class"; break;
  case ONIGERR_TARGET_OF_REPEAT_OPERATOR_NOT_SPECIFIED:
    p = "target of repeat operator is not specified"; break;
  case ONIGERR_TARGET_OF_REPEAT_OPERATOR_INVALID:
    p = "target of repeat operator is invalid"; break;
  case ONIGERR_NESTED_REPEAT_OPERATOR:
    p = "nested repeat operator"; break;
  case ONIGERR_UNMATCHED_CLOSE_PARENTHESIS:
    p = "unmatched close parenthesis"; break;
  case ONIGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS:
    p = "end pattern with unmatched parenthesis"; break;
  case ONIGERR_END_PATTERN_IN_GROUP:
    p = "end pattern in group"; break;
  case ONIGERR_UNDEFINED_GROUP_OPTION:
    p = "undefined group option"; break;
  case ONIGERR_INVALID_POSIX_BRACKET_TYPE:
    p = "invalid POSIX bracket type"; break;
  case ONIGERR_INVALID_LOOK_BEHIND_PATTERN:
    p = "invalid pattern in look-behind"; break;
  case ONIGERR_INVALID_REPEAT_RANGE_PATTERN:
    p = "invalid repeat range {lower,upper}"; break;
  case ONIGERR_TOO_BIG_NUMBER:
    p = "too big number"; break;
  case ONIGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE:
    p = "too big number for repeat range"; break;
  case ONIGERR_UPPER_SMALLER_THAN_LOWER_IN_REPEAT_RANGE:
    p = "upper is smaller than lower in repeat range"; break;
  case ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS:
    p = "empty range in char class"; break;
  case ONIGERR_MISMATCH_CODE_LENGTH_IN_CLASS_RANGE:
    p = "mismatch multibyte code length in char-class range"; break;
  case ONIGERR_TOO_MANY_MULTI_BYTE_RANGES:
    p = "too many multibyte code ranges are specified"; break;
  case ONIGERR_TOO_SHORT_MULTI_BYTE_STRING:
    p = "too short multibyte code string"; break;
  case ONIGERR_TOO_BIG_BACKREF_NUMBER:
    p = "too big backref number"; break;
  case ONIGERR_INVALID_BACKREF:
#ifdef USE_NAMED_GROUP
    p = "invalid backref number/name"; break;
#else
    p = "invalid backref number"; break;
#endif
  case ONIGERR_NUMBERED_BACKREF_OR_CALL_NOT_ALLOWED:
    p = "numbered backref/call is not allowed. (use name)"; break;
  case ONIGERR_TOO_MANY_CAPTURES:
    p = "too many captures"; break;
  case ONIGERR_TOO_BIG_WIDE_CHAR_VALUE:
    p = "too big wide-char value"; break;
  case ONIGERR_TOO_LONG_WIDE_CHAR_VALUE:
    p = "too long wide-char value"; break;
  case ONIGERR_INVALID_CODE_POINT_VALUE:
    p = "invalid code point value"; break;
  case ONIGERR_EMPTY_GROUP_NAME:
    p = "group name is empty"; break;
  case ONIGERR_INVALID_GROUP_NAME:
    p = "invalid group name <%n>"; break;
  case ONIGERR_INVALID_CHAR_IN_GROUP_NAME:
#ifdef USE_NAMED_GROUP
    p = "invalid char in group name <%n>"; break;
#else
    p = "invalid char in group number <%n>"; break;
#endif
  case ONIGERR_UNDEFINED_NAME_REFERENCE:
    p = "undefined name <%n> reference"; break;
  case ONIGERR_UNDEFINED_GROUP_REFERENCE:
    p = "undefined group <%n> reference"; break;
  case ONIGERR_MULTIPLEX_DEFINED_NAME:
    p = "multiplex defined name <%n>"; break;
  case ONIGERR_MULTIPLEX_DEFINITION_NAME_CALL:
    p = "multiplex definition name <%n> call"; break;
  case ONIGERR_NEVER_ENDING_RECURSION:
    p = "never ending recursion"; break;
  case ONIGERR_GROUP_NUMBER_OVER_FOR_CAPTURE_HISTORY:
    p = "group number is too big for capture history"; break;
  case ONIGERR_INVALID_CHAR_PROPERTY_NAME:
    p = "invalid character property name {%n}"; break;
  case ONIGERR_NOT_SUPPORTED_ENCODING_COMBINATION:
    p = "not supported encoding combination"; break;
  case ONIGERR_INVALID_COMBINATION_OF_OPTIONS:
    p = "invalid combination of options"; break;
  case ONIGERR_LIBRARY_IS_NOT_INITIALIZED:
    p = "library is not initialized"; break;

  default:
    p = "undefined error code"; break;
  }

  return (UChar* )p;
}