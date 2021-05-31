#include "fonction.h"

// Initialise le depot
bool init() 
{
    std::cout << "Repository is being initialized..." << std::endl;


    //  > Creer le dossier '.git' s'il n'existe pas
    if (!pathExists(GIT_PATH))
	{
		boost::filesystem::create_directory(GIT_PATH);
	}

    //  > Faire dossier 'objects' s'il n'existe pas
    if (!pathExists(GIT_PATH + "objects"))
	{
		boost::filesystem::create_directory(GIT_PATH + "objects");
	}
    
    //  > Ajouter fichier 'HEAD'
    writeFile(GIT_PATH + "HEAD", "");

    //  > Ajouter fichier 'index'
    writeFile(GIT_PATH + "index", "");

    //
    //----------NON-DEMANDES----------
    //
    //     > Dossier 'refs'
    //     > Dossier 'info'
    //     > Fichier 'config'
    //     > Fichier 'description'
    
    std::cout << "Repository has been initialized successfully!" << std::endl;
    return true;
}

bool add(std::string filePath) 
{
    // Verifier si le fichier existe
    if (!fileExists(filePath))
	{
        std::cout << "The path does not point to an existing file" << std::endl;
        return false;
    }

    // Verifier si le dossier objects existe
    if (!pathExists(GIT_PATH + "objects"))
	{
        std::cout << "The folder \"objects\" does not exist" << std::endl << "Did you forget to call gitus init first?" << std::endl;
        return false;
	}

    // Lecture du fichier a add
    std::string file = readFile(filePath);

    // Generation d'un SHA1 a partir du contenu du fichier
    std::string sha1 = getSHA(file);   
    std::cout << "sha1 : " << sha1 << std::endl;    
    std::string shaPath = GIT_PATH + "objects/" + sha1.substr(0, 2) + "/";

    // Un enregistrement local est cree contenant les informations utiles sur le fichier
    boost::filesystem::create_directory(shaPath);

    // Formattage des informations contenues dans le fichier
    std::string stagingText = filePath + "\n\n" + std::to_string(file.length()) + "\n\n" + file;

    // Ecriture du fichier dans un dossier contenant les 2 premiers caracteres du SHA1
    writeFile(shaPath + sha1.substr(2), stagingText);


    // Ecriture dans l'index
    std::string index = sha1 + " " + filePath + "\n";

    if (writeIndexFile(index))
    {
        std::cout << "The file " + filePath + " has been added succesfully" << std::endl;
        return true;
    }
    else
    {
        std::cout << "Something went wrong while trying to add the file " + filePath << std::endl << "Chances are that no modification has been made to the file since the last time it has been added to the repo" << std::endl;
        return false;
    }
    
}

bool commit(std::string msg, std::string author) 
{
    // Verifie si le message ou l'auteur est vide
    if (msg.empty())
    {
        std::cout << "The message is empty. For more information, use the method \"gitus commit --help\"" << std::endl;
        return false;
    }
    if (author.empty())
    {
        std::cout << "The author is empty. For more information, use the method \"gitus commit --help\"" << std::endl;
        return false;
    }
    
    //  Verifie si le depot a ete initialise en verifiant la presence du fichier index
    if (!indexExists())
	{
        std::cout << "The index file is missing. To initialize the repo, use the method \"gitus init\" first?" << std::endl;
        return false;
	}

    //  1. Lire les elements dans .git/index
    std::string elements = readFile(GIT_PATH + "index");

    //  Verifie s'il y a des fichiers prets a etre sauvegarde dans un commit
    if (elements.empty())
	{
        std::cout << "There are no file to commit. To add a file to a commit, use the method \"gitus add <pathspec>\" first." << std::endl;
        return false;
	}
  
    //  Obtention du SHA1 du HEAD
    std::string lastCommitSHA1 = readFile(GIT_PATH + "HEAD");

    //  Si le SHA1 du HEAD est vide, ça veut dire qu'on fait le premier commit
    // 2. Creer un arbre à partir du root
    if (lastCommitSHA1.empty() && !pathExists(GIT_PATH + "tree"))
	{
        boost::filesystem::create_directory(GIT_PATH + "tree");
    }
    
    std::string newTree = getMergedTree(elements);

    std::string sha1Tree = getSHA(newTree);
    // Le nouveau fichier dans l'arbre correspond à l'index du commit et son nom est le SHA1 genere a partir de l'index
    writeFile(GIT_PATH + "tree/" + sha1Tree, newTree);

    // 3. Creer un commit et l'ecrire dans la base de donnees
    //     a. Auteur
    //     b. Commit parent
    //     c. SHA1 de l'arbre
    //     d. Date (h:m:s)
    //     e. Message
    std::string commit = author + '\n' + lastCommitSHA1 + '\n' + sha1Tree + '\n' + getCurrTime() + '\n' + msg + '\n';
    std::string sha1Commit = getSHA(commit);
    std::string sha1Path = GIT_PATH + "objects/" + sha1Commit.substr(0, 2) + "/";

    boost::filesystem::create_directory(sha1Path);
    writeFile(sha1Path + sha1Commit.substr(2), commit);


    // 4. Eliminer les changements dans le .git/index
    writeFile(GIT_PATH + "index", "");
    
    // 5. Inserer le SHA1 du commit dans le fichier .git/HEAD
    writeFile(GIT_PATH + "HEAD", sha1Commit);
    return true;
}

void checkout(std::string commitID) 
{
    std::cout << "Not yet implemented" << std::endl;
}