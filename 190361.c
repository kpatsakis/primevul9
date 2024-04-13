void generate_fields(std::ofstream& out,
                     const vector<t_field*>& fields,
                     const std::string& indent) {
  const vector<t_field*>::const_iterator beg = fields.begin();
  const vector<t_field*>::const_iterator end = fields.end();

  for (vector<t_field*>::const_iterator it = beg; it != end; ++it) {
    out << indent << "out << ";

    if (it != beg) {
      out << "\", \" << ";
    }

    generate_field(out, *it);
    out << ";" << endl;
  }
}