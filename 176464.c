char opening_bracket_for(char closing_bracket) {
  switch (closing_bracket) {
    case ')': return '(';
    case ']': return '[';
    case '}': return '{';
    default: return '\0';
  }
}