void t_cpp_generator::generate_service_skeleton(t_service* tservice) {
  string svcname = tservice->get_name();

  // Service implementation file includes
  string f_skeleton_name = get_out_dir() + svcname + "_server.skeleton.cpp";

  string ns = namespace_prefix(tservice->get_program()->get_namespace("cpp"));

  ofstream f_skeleton;
  f_skeleton.open(f_skeleton_name.c_str());
  f_skeleton << "// This autogenerated skeleton file illustrates how to build a server." << endl
             << "// You should copy it to another filename to avoid overwriting it." << endl << endl
             << "#include \"" << get_include_prefix(*get_program()) << svcname << ".h\"" << endl
             << "#include <thrift/protocol/TBinaryProtocol.h>" << endl
             << "#include <thrift/server/TSimpleServer.h>" << endl
             << "#include <thrift/transport/TServerSocket.h>" << endl
             << "#include <thrift/transport/TBufferTransports.h>" << endl << endl
             << "using namespace ::apache::thrift;" << endl
             << "using namespace ::apache::thrift::protocol;" << endl
             << "using namespace ::apache::thrift::transport;" << endl
             << "using namespace ::apache::thrift::server;" << endl << endl
             << "using boost::shared_ptr;" << endl << endl;

  // the following code would not compile:
  // using namespace ;
  // using namespace ::;
  if ((!ns.empty()) && (ns.compare(" ::") != 0)) {
    f_skeleton << "using namespace " << string(ns, 0, ns.size() - 2) << ";" << endl << endl;
  }

  f_skeleton << "class " << svcname << "Handler : virtual public " << svcname << "If {" << endl
             << " public:" << endl;
  indent_up();
  f_skeleton << indent() << svcname << "Handler() {" << endl << indent()
             << "  // Your initialization goes here" << endl << indent() << "}" << endl << endl;

  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    generate_java_doc(f_skeleton, *f_iter);
    f_skeleton << indent() << function_signature(*f_iter, "") << " {" << endl << indent()
               << "  // Your implementation goes here" << endl << indent() << "  printf(\""
               << (*f_iter)->get_name() << "\\n\");" << endl << indent() << "}" << endl << endl;
  }

  indent_down();
  f_skeleton << "};" << endl << endl;

  f_skeleton << indent() << "int main(int argc, char **argv) {" << endl;
  indent_up();
  f_skeleton
      << indent() << "int port = 9090;" << endl << indent() << "shared_ptr<" << svcname
      << "Handler> handler(new " << svcname << "Handler());" << endl << indent()
      << "shared_ptr<TProcessor> processor(new " << svcname << "Processor(handler));" << endl
      << indent() << "shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));"
      << endl << indent()
      << "shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());" << endl
      << indent() << "shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());"
      << endl << endl << indent()
      << "TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);"
      << endl << indent() << "server.serve();" << endl << indent() << "return 0;" << endl;
  indent_down();
  f_skeleton << "}" << endl << endl;

  // Close the files
  f_skeleton.close();
}