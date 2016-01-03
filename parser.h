#ifndef PARSER_H
#define PARSER_H

#include <string>

class parser{
public:
    parser() { }
    
    void parse(std::string& s, char* addr, unsigned char& subid){
      std::string delimiter = "/\\";
//      size_t pos = 0;
//      std::string token;
//        
//      while ((pos = s.find(delimiter)) != std::string::npos) {
//          token = s.substr(0, pos);
//          std::cout << token << std::endl;
//        s.erase(0, pos + delimiter.length());
//      }
//      std::cout << s << std::endl;

      std::size_t found = s.find_last_of(delimiter);
      //std::cout << " addr: " << s.substr(0,found) << '\n';
      //std::cout << " id: " << s.substr(found+1) << '\n';
      //strcpy(addr, s.substr(0,found).c_str());
      
      std::string str = s.substr(0,found);
      std::copy(str.begin(), str.end(), addr);
      addr[str.size()] = '\0';
      //std::cout << " addr: " << addr << "\n";
      
      subid = static_cast<unsigned char>(std::stoi( s.substr(found+1) )) ;
      //std::cout << " id: " << static_cast<unsigned>(id) << "\n";

    }

};

#endif