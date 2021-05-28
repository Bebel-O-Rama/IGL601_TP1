#include <iostream>
#include <string>
#include <cstring>
#include <utilitary.h>

#include <boost/uuid/detail/sha1.hpp>

//---------------------------
//METTRE DANS UN HEADER

// boost::system::error_code ec; // Pour eviter les exceptions

void init();

void add(std::string);

void commit(std::string, std::string);

void checkout(std::string);

void writeFile(std::string filePath, std::string fileText, bool isAppend = false);
void writeIndexFile(std::string indexPath, std::string fileText);
std::string readFile(std::string filePath);

bool pathExists(std::string path);
bool fileExists(std::string filePath);
bool indexExists();

void createTreeNode(std::string prevSHA1, std::string currSHA1, std::string * modifiedFilesSHA1);
//---------------------------


const std::string GIT_PATH = boost::filesystem::current_path().append(".git/").string();

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

// Initialise le depot
void init() 
{
    std::cout << "Repository is being initialized..." << std::endl;


    // > Creer le dossier '.git' s'il n'existe pas
    if (pathExists(GIT_PATH))
	{
		boost::filesystem::create_directory(GIT_PATH);
	}

    //     > Faire dossier 'objects' s'il n'existe pas
    if (!pathExists(GIT_PATH + "objects"))
	{
		boost::filesystem::create_directory(GIT_PATH + "objects");
	}

    //     > Creer dossier 'heads' (contenant 'main' et autres branches) s'il n'existe pas
    if (!pathExists(GIT_PATH + "heads"))
	{
		boost::filesystem::create_directory(GIT_PATH + "heads");
	}

    //     ­­   > Creer fichier 'main' (ou 'master')
    writeFile(GIT_PATH + "heads/main", getRandSHA());
    
    //     > Ajouter fichier 'HEAD'
    writeFile(GIT_PATH + "HEAD", "ref: heads/main");

    //     > Ajouter fichier 'index'
    writeFile(GIT_PATH + "index", "");

    //
    //----------NON-DEMANDES----------
    //
    //     > Dossier 'refs'
    //     > Dossier 'info'
    //     > Fichier 'config'
    //     > Fichier 'description'
    
    std::cout << "Repository has been initialized successfully!" << std::endl;
}

// Lit un fichier et retourne son contenu
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

// Ecrit dans un fichier. On peut choisir d'ajouter ou d'ecraser le contenu. Ecrase par defaut
void writeFile(std::string filePath, std::string fileText, bool isAppend)
{
    using std::ofstream;
    
    std::ofstream ofs;

    if (isAppend)
    {
        ofs = std::ofstream(filePath, std::ofstream::app);    
    }
    else
    {
        ofs = std::ofstream(filePath, std::ofstream::out);
    }

    ofs << fileText;

    ofs.close();
}

// Ecrit dans le fichier index situe a 'indexPath' le text 'fileText'
void writeIndexFile(std::string fileText)
{
    const std::string indexPath = GIT_PATH + "index";
    std::string index = readFile(indexPath);
    int pos = index.find(fileText);

    if (pos != std::string::npos)
    {       
        for (int i = pos; i < fileText.length() + pos; ++i)
        {
            index[i] = fileText[i - pos];
        }
        writeFile(indexPath, index);
    }
    else
    {
        writeFile(indexPath, fileText, true);
    }
}

void add(std::string filePath) 
{
    // Verifier si le fichier existe
    if (!fileExists(filePath))
	{
        // if (ec.failed())
		// {
        std::cout << "The path does not point to an existing file" << std::endl;
        return;
		// }
    }


    // Verifier si le dossier objects existe
    if (!pathExists(GIT_PATH + "objects"))
	{
        std::cout << "The folder \"objects\" does not exist" << std::endl << "Did you forget to call gitus init first?" << std::endl;
        return;
	}


    // Lecture du fichier a add
    std::string file = readFile(filePath);


    // Generation d'un SHA1 a partir du contenu du fichier
    std::string sha1 = getSHA(file);      
    std::string shaPath = GIT_PATH + "objects/" + sha1.substr(0, 2) + "/";


    // Un enregistrement local est cree contenant les informations utiles sur le fichier
    boost::filesystem::create_directory(shaPath);
    std::string fileName;
    

    // Obtention du nom du fichier
    int pos = filePath.find_last_of("/");
    if (pos == -1)
    {
        fileName = filePath;
    }
    else
    {
        fileName = filePath.substr(pos+1);
    }


    // Formattage des informations contenues dans le fichier
    std::string stagingText = fileName + "\n\n" + std::to_string(file.length()) + "\n\n" + file;


    // Ecriture du fichier dans un dossier contenant les 2 premiers caracteres du SHA1
    writeFile(shaPath + sha1.substr(2), stagingText);


    // Ecriture dans l'index
    std::string index = sha1 + " " + filePath + "\n";

    writeIndexFile(index);


    std::cout << "The file " + filePath + " has been added succesfully" << std::endl;
}

void commit(std::string msg, std::string author) 
{
    //      Verifie si le depot a ete initialise en verifiant la presence du fichier index
    if (!indexExists())
	{
        return;
	}

    std::string commitSHA1 = getRandSHA();

    //      Obtention de l'emplacement du dernier commit à partir de HEAD
    std::string headRef = readFile(GIT_PATH + "HEAD").substr(5);
    std::string lastCommitSHA1 = readFile(GIT_PATH + headRef);


    // 1. Lire les elements dans .git/index
    std::string elements = readFile(GIT_PATH + "index");


    // 2. Creer un arbre à partir du root
    if (!pathExists(GIT_PATH + "tree"))
	{
        boost::filesystem::create_directory(GIT_PATH + "tree");

        std::cout << "gets in there";
    }

    // Le nom de l'arbre correspond au commit auquel il fait reference
    // Son contenu represente le SHA1 de son predesceceur
    writeFile(GIT_PATH + "tree/" + commitSHA1, lastCommitSHA1);

    // 3. Creer un commit et l'ecrire dans la base de donnees
    //     a. Auteur
    //     b. Commit parent
    //     c. SHA1 de l'arbre
    //     d. Date (h:m:s)
    //     e. Message
    if (!pathExists(GIT_PATH + "commits"))
	{
        boost::filesystem::create_directory(GIT_PATH + "commits");
    }
    std::string commit = author + '\n' + lastCommitSHA1 + '\n' + commitSHA1 + '\n' + getCurrTime() + '\n' + msg + '\n';

    writeFile(GIT_PATH + "commits/" + commitSHA1, commit);


    // 4. Eliminer les changements dans le .git/index
    writeIndexFile("");
    

    ////////////////////////////////////////////////////////
    //  A REVOIR: SI ON ECRIT DIRECT LE COMMIT DANS HEAD  //
    ////////////////////////////////////////////////////////

    // 5. Inserer le SHA1 du commit dans le fichier .git/HEAD
    // ­> On ecrira plutot la reference vers ce commit dans HEAD
    //   Cela simplifiera un peu les choses pour l'implementation du checkout
    writeFile(GIT_PATH + headRef, commitSHA1);

}

void checkout(std::string commitID) 
{
    std::cout << "Not yet implemented" << std::endl;
}

void createTreeNode(std::string prevSHA1, std::string currSHA1, std::string * modifiedFilesSHA1)
{

}

bool pathExists(std::string path) 
{
    boost::system::error_code ec; // Pour eviter les exceptions

    return boost::filesystem::is_directory(path, ec);
}

bool fileExists(std::string filePath) 
{
    boost::system::error_code ec; // Pour eviter les exceptions

    return boost::filesystem::exists(filePath, ec);
}

bool indexExists() 
{
    if (!fileExists(GIT_PATH + "index"))
	{
        std::cout << "The file \"index\" does not exist" << std::endl << "Did you forget to call gitus init first?" << std::endl;
        return false;
	}
    return true;
}