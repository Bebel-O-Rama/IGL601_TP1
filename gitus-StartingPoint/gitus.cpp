#include "gitus.h"

#include "utilitary.h"
#include "fonction.cpp"

// Le main est le point d'entree du programme
int main(int argc, char * arcv[])
{
    // Determine si l'utilisateur a entre une commande
    if(argc > 1)
    {
        if(strcmp(arcv[1], "--help") == 0)
        {
            std::cout << "usage: gitus <command> [<args>]" << std::endl <<
            "These are common gitus commands used in various situations:" << std::endl <<
            "init       Create an empty Git repository or reinitialize an existing one" << std::endl <<
            "add        Add file contents to the index" << std::endl <<
            "commit     Record changes to the repository" << std::endl;
        }
        else if(strcmp(arcv[1], "init") == 0)
        {
            if (argc == 3)
            {
                if (strcmp(arcv[2], "--help") == 0)
                {
                    std::cout << "usage: gitus init" << std::endl;
                }
                else
                {
                    std::cout << "Unknown argument for the command \"init\"" << std::endl << "For more information, please refer to \"./gitus init --help\"" << std::endl;
                }
            }
            else if (argc == 2)
            {
                init();
            }
            else
            {
                std::cout << "Unknown argument for the command \"init\"" << std::endl << "For more information, please refer to \"./gitus init --help\"" << std::endl;
            }
        }
        else if(strcmp(arcv[1], "add") == 0)
        {
            if (argc == 3)
            {
                if (strcmp(arcv[2], "--help") == 0)
                {
                    std::cout << "usage: gitus add <pathspec>" << std::endl;
                }
                else
                {
                    add(arcv[2]);
                }
            }
            else
            {
                std::cout << "Unknown argument for the command \"add\"" << std::endl << "For more information, please refer to \"./gitus add --help\"" << std::endl;
            }
        }
        else if(strcmp(arcv[1], "commit") == 0)
        {
            if (argc == 3)
            {
                if (strcmp(arcv[2], "--help") == 0)
                {
                    std::cout << "usage: gitus commit <msg> <author>" << std::endl;
                }
                else
                {
                    std::cout << "Unknown argument for the command \"commit\"" << std::endl << "For more information, please refer to \"./gitus commit --help\"" << std::endl;
                }
            }
            else if (argc == 4)
            {
                commit(arcv[2], arcv[3]);
            }  
            else
            {
                std::cout << "Unknown argument for the command \"commit\"" << std::endl << "For more information, please refer to \"./gitus commit --help\"" << std::endl;
            }
        }
        else if(strcmp(arcv[1], "checkout") == 0)
        {
            if (argc == 3)
            {
                if (strcmp(arcv[2], "--help") == 0)
                {
                    std::cout << "usage: gitus checkout <commitID>" << std::endl;
                }
                else
                {
                    checkout(arcv[2]);
                }
            }
            else
            {
                std::cout << "Unknown argument for the command \"checkout\"" << std::endl << "For more information, please refer to \"./gitus checkout --help\"" << std::endl;
            }
        }
        else
        {
            std::cout << "The command does not exist." << std::endl << "For more information, use \"./gitus --help\"" << std::endl;
        }
    }
    else
    {
        std::cout << "Please, enter a command following \"./gitus\"" << std::endl << "For more information, use \"./gitus --help\"" << std::endl;
    }
    
    return 0;
}

