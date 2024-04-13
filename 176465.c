char closing_bracket_for(char opening_bracket) {
  switch (opening_bracket) {
    case '(': return ')';
    case '[': return ']';
    case '{': return '}';
    default: return '\0';
  }
}