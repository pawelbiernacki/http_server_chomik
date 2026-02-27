#include "config.h"
#include <http_chomik.h>
#include "http_server_chomik.h"
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>

#define HTTP_CHOMIK_PRODUCTION

#ifdef HTTP_CHOMIK_PRODUCTION
#define HTTP_CHOMIK_LOG_ERROR(X)    \
{                                   \
    std::stringstream s;            \
    s << X;                         \
    syslog (LOG_PERROR, s.str().c_str());   \
}
#define HTTP_CHOMIK_LOG_NOTICE(X)   \
{                                   \
    std::stringstream s;            \
    s << X;                         \
    syslog (LOG_NOTICE, s.str().c_str());   \
}
#else
#define HTTP_CHOMIK_LOG_ERROR(X)    \
{                                   \
    std::stringstream s;            \
    s << X;                         \
    std::cerr << s.str() << "\n";   \
}
#define HTTP_CHOMIK_LOG_NOTICE(X)   \
{                                   \
    std::stringstream s;            \
    s << X;                         \
    std::cout << s.str() << "\n";   \
}
#endif


int my_port = 5001;
bool my_port_known = false;


void http_server_chomik::machine::create_predefined_variables()
{
    chomik::machine::create_predefined_variables();
}


bool http_server_chomik::machine::get_is_user_defined_executable(const chomik::signature & s) const
{
    if (s.get_vector_of_items().size() == 3)
    {
        if (s.get_vector_of_items()[0]->get_it_is_identifier("http")
            && s.get_vector_of_items()[1]->get_it_is_identifier("server")
            && s.get_vector_of_items()[2]->get_it_is_identifier("loop"))
        {
            return true;
        }
    }
    return false;
}

void http_server_chomik::machine::http_server_loop()
{
    HTTP_CHOMIK_LOG_NOTICE("http_chomik started.");
    pid_t f, pid;

    f = fork();
    if (f < 0)
    {
        exit(EXIT_FAILURE);
    }

    if (setsid() < 0)
        exit(EXIT_FAILURE);

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    pid = fork();

    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    umask(0);

    chdir("/root");

    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }

    openlog("chomik", LOG_PID|LOG_CONS, LOG_USER);

    syslog(LOG_INFO, PACKAGE_STRING " daemon is running");

    http_chomik::wrapper<http_chomik::server> my_wrapper;
    http_chomik::server::do_processing(my_port, my_wrapper);
}

void http_server_chomik::machine::execute_user_defined_executable(const chomik::signature & s)
{
    if (s.get_vector_of_items().size() == 3)
    {
        if (s.get_vector_of_items()[0]->get_it_is_identifier("http")
            && s.get_vector_of_items()[1]->get_it_is_identifier("server")
            && s.get_vector_of_items()[2]->get_it_is_identifier("loop"))
        {
            http_server_loop();
        }
    }
}

chomik::program the_program;
chomik::parser the_parser{the_program};


int main(int argc, char *argv[])
{
    int return_value = 0;
    std::string filename="";

    chomik::parser::register_parser(&the_parser);

    for (int a=1; a < argc; a++)
    {
        std::string option(argv[a]);
        if (option=="-p" || option=="--port")
        {
            if (a == argc-1)
            {
                std::cerr << "option -p or --port require a port number\n";
                return -1;
            }
            std::string port(argv[a+1]);
            a++;
            my_port = std::stoi(port);
            my_port_known = true;
        }
        else
        if (option == "-h" || option == "--help")
        {
            std::cout << "usage: " << argv[0] << " [-p|--port <port>] [-h|--help] <filename>\n";
            return 0;
        }
        else
        {
            filename = argv[a];
        }
    }

    if (!my_port_known)
    {
        std::cerr << "unknown port\n";
        return_value = 1;
    }
    else
    if (filename == "")
    {
        std::cerr << "usage: " << argv[0] << " <filename>\n";
        return_value = 1;
    }
    else
    {
        if (the_parser.parse(filename.c_str()) == 0)
        {
            http_server_chomik::machine m;
            m.create_predefined_types();
            m.create_predefined_variables();
            m.create_predefined_streams();

            the_program.execute(m);


            chomik::generic_name gn;
            gn.add_generic_name_item(std::make_shared<chomik::identifier_name_item>("the"));
            gn.add_generic_name_item(std::make_shared<chomik::identifier_name_item>("program"));
            gn.add_generic_name_item(std::make_shared<chomik::identifier_name_item>("return"));
            chomik::signature s0{gn};

            return_value = m.get_variable_value_integer(s0);
        }
    }

    return return_value;
}
