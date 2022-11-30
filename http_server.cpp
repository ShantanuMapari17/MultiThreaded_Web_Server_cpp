#include "http_server.hh"

#include <vector>

#include <sys/stat.h>

#include <fstream>
#include <sstream>
#include<string>

using namespace std;

vector<string> split(const string &s, char delim) {
  vector<string> elems;

  stringstream ss(s);
  string item;

  while (getline(ss, item, delim)) {
    if (!item.empty())
      elems.push_back(item);
  }

  return elems;
}

HTTP_Request::HTTP_Request(string request) {
  vector<string> lines = split(request, '\n');
  vector<string> first_line = split(lines[0], ' ');

  this->HTTP_version = "1.0"; // We'll be using 1.0 irrespective of the request

  /*
   TODO : extract the request method and URL from first_line here
    */
  this->url=first_line[1];
  this->method=first_line[0];

  if (this->method != "GET") {
    cerr << "Method '" << this->method << "' not supported" << endl;
    exit(1);
  }
}

HTTP_Response *handle_request(string req) {
  HTTP_Request *request = new HTTP_Request(req);

  HTTP_Response *response = new HTTP_Response();
  
  string url = string("html_files") + request->url;

  response->HTTP_version = "1.0";

  struct stat sb;
  if (stat(url.c_str(), &sb) == 0) // requested path exists
  {
    response->status_code = "200";
    response->status_text = "OK";
    response->content_type = "text/html";

    string body;

    if (S_ISDIR(sb.st_mode)) {
      /*
      In this case, requested path is a directory.
      TODO : find the index.html file in that directory (modify the url
      accordingly)
      */

      string valid_index_html=url+"/index.html";

      if(stat(valid_index_html.c_str(),&sb)==0){
        url=valid_index_html;
      }
      else{

        
        response->status_code = "404";

        /*
        TODO : set the remaining fields of response appropriately
        */
        response->status_text="Not Found";
        response->content_type = "text/html";

        ifstream myfile ("error.html");
        auto my_string=ostringstream{};
        string ans;
        my_string << myfile.rdbuf();
        ans=my_string.str();
        // 
        cout<<ans<<endl;

        response->body=ans.c_str();
        response->content_length=to_string(response->body.length());

        delete request;

        return response;

      }
    }

    /*
    TODO : open the file and read its contents
    */

    ifstream myfile (url.c_str());
    auto my_string=ostringstream{};
    string ans;
    my_string << myfile.rdbuf();
    ans=my_string.str();
    // cout<<ans<<endl;
    



    /*
    TODO : set the remaining fields of response appropriately
    */

    response->body=ans.c_str();
    response->content_length=to_string(response->body.length());
    // cout<<response->content_length<<endl;

  }

  else {
    response->status_code = "404";

    /*
    TODO : set the remaining fields of response appropriately
    */
    response->status_text="Not Found";
    response->content_type = "text/html";

    ifstream myfile ("error.html");
    auto my_string=ostringstream{};
    string ans;
    my_string << myfile.rdbuf();
    ans=my_string.str();
    // 
    // cout<<ans<<endl;

    response->body=ans.c_str();
    response->content_length=to_string(response->body.length());

    
  }

  delete request;

  return response;
}

string HTTP_Response::get_string() {
  /*
  TODO : implement this function
  */
 string ans;
 string statusline="HTTP/"+this->HTTP_version+" "+this->status_code+" "+this->status_text+"\n";
 string c_type="Content-Type: "+this->content_type+"\n";
 string c_length="Content-Length: "+this->content_length+"\n\n";
 ans=statusline+c_type+c_length+this->body;



 return ans;
}
