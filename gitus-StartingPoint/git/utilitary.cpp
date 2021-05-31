#include "utilitary.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/uuid/detail/sha1.hpp>


// cette fonction est d�di�e � �tre �limin�e -- seulement de test


//---------------------------------------------
// le sha
// https://pragmaticjoe.blogspot.com/2015/02/how-to-generate-sha1-hash-in-c.html
std::string getSHA(std::string inputStr)
{
	using boost::uuids::detail::sha1;
	using std::ifstream;

	sha1 sha;

	sha.process_bytes(inputStr.c_str(), inputStr.length());

	unsigned int hash[5];
	sha.get_digest(hash);
	
	std::stringstream stream;

	for (int i = 0; i < 5; ++i) {
		stream << std::hex << hash[i]; 
	}

	inputStr = stream.str();

	return inputStr;
}

std::string getRandSHA()
{
	std::string sha1 = getSHA(getCurrTime()); 

	//A commenter si on en veut plus
	std::cout << sha1 << std::endl;

    return sha1;
}

std::string getCurrTime()
{
	time_t timer = time_t();
    const time_t currTime = time(&timer);
	std::string timeNow = ctime(&currTime); 

	//A commenter si on en veut plus
	std::cout << timeNow << std::endl;

    return timeNow;
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



// Ecrit dans le fichier index situe a 'indexPath' le text 'fileText'. S'il s'agit d'un commit (par défaut non) on n'ajoute pas si le fichier est déjà présent
bool writeIndexFile(std::string fileText, bool isCommit)
{
    const std::string indexPath = GIT_PATH + "index";

    std::string index = readFile(indexPath);
	int posDelimiter = fileText.find(" ");
	std::string fileName = fileText.substr(posDelimiter+1);
	int posIndexFileName = index.find(fileName);

	// Si le fichier ne fait pas deja partie de l'index, on peut l'ajouter
	if (posIndexFileName == std::string::npos)
	{
        index += fileText;
		writeFile(indexPath, index);
		//writeFile(indexPath, fileText, true);
		return true;
	}
	
	// On continue uniquement si l'ajout ne vient pas d'un commit anterieur
	if (!isCommit)
	{
		// On continue uniquement si le fichier a ete modifie depuis la derniere fois qu'il a ete ajoute au repo (on regarde si le sha1 est le meme)
		if (index.find(fileText) == std::string::npos)
		{
			int lineDelimiterA = 0; 
			int lineDelimiterB = 0;

			do {
				lineDelimiterA = lineDelimiterB;
				lineDelimiterB = index.find('\n', lineDelimiterA+1);
			}
			while (lineDelimiterB < posIndexFileName);

			index.erase(lineDelimiterA, lineDelimiterB - lineDelimiterA);
			index += fileText;
			if (index.at(0) == '\n')
			{
				index.erase(0, 1);
			}
			writeFile(indexPath, index);
			return true;
		}

		// On retourne faux comme il n'y a pas de nouveau fichier a ajouter
		return false;
	}

	// On retourne faux comme le fichier est deja present, donc rien est change dans l'index
	return false;
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

std::string getMergedTree(std::string currentIndex)
{
	// S'il s'agit du premier commit, il n'y aura forcement pas de difference. On peut donc sortir
	std::string headSHA1 = readFile(GIT_PATH + "HEAD") ;
	if (headSHA1.empty())
	{
		return currentIndex;
	}
	
	// Il faut aller chercher le contenu du tree du commit precedent
	std::string oldCommit = readFile(GIT_PATH + "objects/" + headSHA1.substr(0,2) + "/" + headSHA1.substr(2));
	int lineDelimiterA = 0;
	int lineDelimiterB = 0;
	for (int i = 0; i < 3; i++)
	{
		lineDelimiterA = lineDelimiterB;
		lineDelimiterB = oldCommit.find('\n', lineDelimiterA+1);
	}

	std::string oldTree = readFile(GIT_PATH + "tree/" + oldCommit.substr(lineDelimiterA+1, lineDelimiterB - lineDelimiterA-1));

	// Il faut ensuite ajouter tout les elements du tree precedent qui sont manquant au tree actuel
	lineDelimiterA = 0;
	lineDelimiterB = 0;
	std::string oldEntry;
	do {
		lineDelimiterA = lineDelimiterB +1;
		lineDelimiterB = oldTree.find('\n', lineDelimiterA);
		if (lineDelimiterA == 1)
		{
			oldEntry = oldTree.substr(lineDelimiterA-1, lineDelimiterB - lineDelimiterA+1);
		}
		else
		{
			oldEntry = oldTree.substr(lineDelimiterA-1, lineDelimiterB - lineDelimiterA+1);
		}
		writeIndexFile(oldEntry, true);
	}
	while (lineDelimiterB < oldTree.length() - 1);
	
	std::string elements = readFile(GIT_PATH + "index");

	return elements;
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