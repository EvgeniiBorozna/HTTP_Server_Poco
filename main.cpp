#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Process.h>
#include <iostream>
#include <string>
#include <vector>

using namespace Poco::Net;
using namespace Poco::Util;
using namespace std;

class MyRequestHandler : public HTTPRequestHandler
{
public:
  void handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
  {
    resp.setStatus(HTTPResponse::HTTP_OK);
    resp.setContentType("text/html");

    ostream& out = resp.send();
    string msg = req.getURI();
    if (msg.find("exit") == 1) {
        Poco::Process::requestTermination(Poco::Process::id());
        cout << "Shutdown command received...\n";
        out << "<h1>Server shutdown</h1>";
        out.flush();
        return;
    }
    size_t det = msg.find("x");
    int rows = stoi(msg.substr(1, det - 1));
    int cols = stoi(msg.substr(det + 1, sizeof(msg) - det - 1));
    cout << rows << " " << cols << endl;
    out << "<table border=1 width=60% text-align=center>";
    for (int i = 1; i <= rows; ++i) {
        out << "<tr>";
        for (int j = 1; j <= cols; ++j) {
            out << "<th>" << i << " " << j << "</th>";
        }
        out << "</tr>";
        //out << "<p>" << i << "</p>";
    }
    out << "</table>";

//    out << "<h1>" << req.getURI() <<"</h1>"
//        << "<p>Count: "  << ++count         << "</p>"
//        << "<p>Host: "   << req.getHost()   << "</p>"
//        << "<p>Method: " << req.getMethod() << "</p>"
//        << req.getVersion();
//        //<< "<p>URI: "    << req.getURI()    << "</p>";
    out.flush();

    //cout << endl
         //<< "Response sent for count=" << count
    cout << "Command: " << req.getURI() << endl;
  }

private:
  static int count;
};

int MyRequestHandler::count = 0;

class MyRequestHandlerFactory : public HTTPRequestHandlerFactory
{
public:
  virtual HTTPRequestHandler* createRequestHandler(const HTTPServerRequest &)
  {
    return new MyRequestHandler;
  }
};

class MyServerApp : public ServerApplication
{
    uint16_t port;
public:
    MyServerApp (uint16_t port) : port(port){}

protected:
  int main(const vector<string> &)
  {
    HTTPServer s(new MyRequestHandlerFactory, ServerSocket(port), new HTTPServerParams);

    s.start();
    cout << endl << "Server started at port: " << port << endl;

    waitForTerminationRequest();  // wait for CTRL-C or kill

    cout << endl << "Shutting down..." << endl;
    s.stop();

    return Application::EXIT_OK;
  }
};

int main(int argc, char** argv)
{
    uint16_t portNumber = stoi(argv[1]);
    MyServerApp app(portNumber);
  //ServerApplication app;
  return app.run(argc, argv);
}
