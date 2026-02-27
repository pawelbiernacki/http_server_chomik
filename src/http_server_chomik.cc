
#include "http_server_chomik.h"
#include <cstring>

void http_server_chomik::machine::create_predefined_variables()
{
    chomik::machine::create_predefined_variables();
}


bool http_server_chomik::machine::get_is_user_defined_executable(const chomik::signature & s) const
{
    return false;
}


void http_server_chomik::machine::execute_user_defined_executable(const chomik::signature & s)
{
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
        filename = argv[a];
    }

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
