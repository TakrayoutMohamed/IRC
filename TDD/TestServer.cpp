#include "./../server/Server.hpp"

int main(const int argc, const char **argv)
{
    (void )(argv);
    (void ) (argc);
    try
    {
        std::cout << "the testes to check if the port working successfully" << std::endl;
        Server run;
        std::cout << "The test where port = \"\" ,and password = \"12asdoldj\" \n";
        run.runServer("12asdoldj","2000");//port not acceptable , port empty , password valid
        if (run.getPort() == 0 || run.getPassword().compare("DefaultPassword") == 0)
        {
            std::cout << "\n                  PASSED SUCCESSFULLY!!" << std::endl;
        }
        else
            std::cout << "\n                  FAILLED!!" << std::endl;
        std::cout << "in class server  PORT = " << run.getPort() << " PASSWORD = " << run.getPassword() << std::endl; 
        std::cout << "**************************************************************************************" << std::endl;
        std::cout << "The test where port = \"100\" ,and password = \"12asdoldj\" \n";
        run.runServer("12asdoldj", "100");//port not acceptable , less than 1024 , password valid
        if (run.getPort() == 0 || run.getPassword().compare("DefaultPassword") == 0)
        {
            std::cout << "\n                  PASSED SUCCESSFULLY!!" << std::endl;
        }
        else
            std::cout << "\n                  FAILLED!!" << std::endl;
        std::cout << "in class server  PORT = " << run.getPort() << " PASSWORD = " << run.getPassword() << std::endl; 
        std::cout << "**************************************************************************************" << std::endl;
        std::cout << "The test where port = \"65536\" ,and password = \"12asdoldj\" \n";
        run.runServer("12asdoldj", "65536");//port not acceptable , more than 1024 , password valid
        if (run.getPort() == 0 || run.getPassword().compare("DefaultPassword") == 0)
        {
            std::cout << "\n                  PASSED SUCCESSFULLY!!" << std::endl;
        }
        else
            std::cout << "\n                  FAILLED!!" << std::endl;
        std::cout << "in class server  PORT = " << run.getPort() << " PASSWORD = " << run.getPassword() << std::endl; 
        std::cout << "**************************************************************************************" << std::endl;
        std::cout << "The test where port = \"65530\" ,and password = \"\" \n";
        run.runServer("", "65530");//password not acceptable , empty password
        if (run.getPort() == 0 || run.getPassword().compare("DefaultPassword") == 0)
        {
            std::cout << "\n                  PASSED SUCCESSFULLY!!" << std::endl;
        }
        else
            std::cout << "\n                  FAILLED!!" << std::endl;
        std::cout << "in class server  PORT = " << run.getPort() << " PASSWORD = " << run.getPassword() << std::endl; 
        std::cout << "**************************************************************************************" << std::endl;
        std::cout << "The test where port = \"65530\" ,and password = \"as1\" \n";
        run.runServer("as1","65530");//password not acceptable , less than 4 chars
        if (run.getPort() == 0 || run.getPassword().compare("DefaultPassword") == 0)
        {
            std::cout << "\n                  PASSED SUCCESSFULLY!!" << std::endl;
        }
        else
            std::cout << "\n                  FAILLED!!" << std::endl;
        std::cout << "in class server  PORT = " << run.getPort() << " PASSWORD = " << run.getPassword() << std::endl; 
        std::cout << "**************************************************************************************" << std::endl;
        std::cout << "The test where port = \"1024\" ,and password = \"as1d\" \n";
        run.runServer("as1d","1024");//valid password and port
        if (run.getPort() == 1024 && run.getPassword().compare("as1d") == 0)
        {
            std::cout << "\n                  PASSED SUCCESSFULLY!!" << std::endl;
        }
        else
            std::cout << "\n                  FAILLED!!" << std::endl;
        std::cout << "in class server  PORT = " << run.getPort() << " PASSWORD = " << run.getPassword() << std::endl; 
        std::cout << "**************************************************************************************" << std::endl;
        std::cout << "The test where port = \"1025\" ,and password = \"as1d\" \n";
        run.runServer("as1d","1025");//valid password and port
        if (run.getPort() == 1025 && run.getPassword().compare("as1d") == 0)
        {
            std::cout << "\n                  PASSED SUCCESSFULLY!!" << std::endl;
        }
        else
            std::cout << "\n                  FAILLED!!" << std::endl;
        std::cout << "in class server  PORT = " << run.getPort() << " PASSWORD = " << run.getPassword() << std::endl; 
    }
    catch(const std::exception& e)
    {
        std::cout << "**********here is the TestServer exception************" << std::endl;
        std::cerr << e.what() << '\n';
    }
    

    return (0);
}