#include <iostream>
#include <string>
#include <cstring>
#include <utilitary.h>

#include <boost/uuid/detail/sha1.hpp>

//---------------------------
//METTRE DANS UN HEADER
boost::system::error_code ec; // Pour éviter les exceptions

void init();
void createObjects();

void add(std::string&);

void commit();

void checkout();

void writeFile(std::string filePath, std::string fileText);
std::string readFile(std::string filePath);
//---------------------------


const std::string GIT_PATH = boost::filesystem::current_path().append(".git/").string();

// The main loop takes the input argument and 
int main(int argc, char * arcv[])
{
    // If the user entered a command
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
                //TODO : Implement gitus init
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
                    //TODO : Implement gitus add
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
                //TODO : Implement gitus commit
                commit();
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
                    //TODO : Implement gitus checkout
                    checkout();
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

//-----------------------------------------
// TODO: Wrapper dans une classe (?)
void init() 
{
    std::cout << "Repository is being initialized..." << std::endl;


    // > Créer le dossier '.git'
    if (!boost::filesystem::exists(GIT_PATH, ec))  // peut lancer une exception
	{
		boost::filesystem::create_directory(GIT_PATH); // exception possible
	}

    //     > Faire dossier 'objects'
    if (!boost::filesystem::exists(GIT_PATH + "objects", ec))  // peut lancer une exception
	{
		boost::filesystem::create_directory(GIT_PATH + "objects"); // exception possible
	}

    //     > Créer dossier 'heads' (contenant 'main' et autres branches)
    if (!boost::filesystem::exists(GIT_PATH + "heads", ec))  // peut lancer une exception
	{
		boost::filesystem::create_directory(GIT_PATH + "heads"); // exception possible
	}

    //     ­­   > Créer fichier 'main' (ou 'master')
    writeFile(GIT_PATH + "heads/main", getRandSHA());
    
    //     > Ajouter fichier 'HEAD'
    writeFile(GIT_PATH + "HEAD", "ref: heads/main");

    //     > Ajouter fichier 'index'
    writeFile(GIT_PATH + "index", "");

    //
    //----------NON-DEMANDÉS----------
    //
    //     > Dossier 'refs'
    //     > Dossier 'info'
    //     > Fichier 'config'
    //     > Fichier 'description'
    
    std::cout << "Repository has been initialized successfully!" << std::endl;
}

void createObjects()
{

}
//-----------------------------------------

std::string readFile(std::string filePath)
{
    using std::ifstream;
	using std::string;

	// lecture du fichier
	ifstream file(filePath);	// pas besoin de gerer l'acces
								// mais il faut s'assurer que le fichier existe
	string content{	std::istreambuf_iterator<char>(file),
					std::istreambuf_iterator<char>() };

    return content;
}

void writeFile(std::string filePath, std::string fileText)
{
    using std::ofstream;

    std::ofstream ofs (filePath, std::ofstream::out);

    ofs << fileText;

    ofs.close();
}

//-----------------------------------------
// A retirer: utiliser utilitary.cpp
//-----------------------------------------

// void writeShaToFile(std::string file) 
// {
//     using boost::uuids::detail::sha1;
// 	using std::ifstream;
// void writeShaToFile(std::string file) 
// {
//     using boost::uuids::detail::sha1;
// 	using std::ifstream;
// 	using std::string;
//
//     sha1 sha;
// 	sha.process_bytes(content.c_str(), content.length());
//
// 	// bof... would prefer array...but the digest does not support it
// 	unsigned int hash[5];
// 	sha.get_digest(hash);
//
// 	std::stringstream stream;
//
// 	std::string result;
// 	// there is something missing here...but what?
// 	for (int i = 0; i < 5; ++i) {
// 		stream << std::hex << hash[i]; 
// 	}
// 	content += stream.str();
// }
// 	using std::string;
//
//     sha1 sha;
// 	sha.process_bytes(content.c_str(), content.length());
//
// 	// bof... would prefer array...but the digest does not support it
// 	unsigned int hash[5];
// 	sha.get_digest(hash);
//
// 	std::stringstream stream;
//
// 	std::string result;
// 	// there is something missing here...but what?
// 	for (int i = 0; i < 5; ++i) {
// 		stream << std::hex << hash[i]; 
// 	}
// 	content += stream.str();
// }

void add(std::string &filePath) 
{
    if (!boost::filesystem::exists(filePath, ec))  // peut lancer une exception
	{
        if (ec.failed())
		{
			std::cout << "The path does not point to an existing file" << std::endl;
            return;
		}
    }
    
    if (!boost::filesystem::exists(GIT_PATH + "objects", ec))  // peut lancer une exception
	{
        std::cout << "The folder \"objects\" does not exist" << std::endl << "Did you forget to call gitus init first?" << std::endl;
        return;
	}

    std::string file = readFile(filePath);

    std::string sha1 = getSHA(file);      
    std::string shaPath = GIT_PATH + "objects/" + sha1.substr(0, 2) + "/";

    boost::filesystem::create_directory(shaPath); // exception possible
    std::string fileName;
    
    int pos = filePath.find_last_of("/");
    if (pos == -1)
    {
        fileName = filePath;
    }
    else
    {
        fileName = filePath.substr(pos);
    }

    std::cout << "find last of : " << pos << std::endl; 

    std::string nbCharacters = file.length().to_string();

    std::string stagingText = fileName + "\n" + std::to_string(file.length()) + "\n" + file;

    writeFile(shaPath + sha1.substr(2), sha1);
    
}

void commit() 
{
    std::cout << "Not yet implemented" << std::endl;
}

void checkout() 
{
    std::cout << "Not yet implemented" << std::endl;
}