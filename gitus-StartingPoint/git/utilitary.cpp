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
void TestDeTrucs() throw(boost::filesystem::filesystem_error) // le throw ici est "deprecated". Il est juste conserver � titre de documentation
{
	/*
		Le but de la fonction est de pr�senter les techniques utiles pour la
		r�alisation du travail
		Ceci inclus:
				l'utilisation de boost::filesystem
				la g�n�ration de sha1
				l'utilisation de zlib (de boost)

		Il manque beaucoup de trucs ici comme, par exemple, la gestion des erreurs
		Ce n'est pas le but de ce fichier de faire ce genre de d�tail
	*/

	using boost::uuids::detail::sha1;
	using std::ifstream;
	using std::string;

	const auto filePath = "truc.txt";

	// lecture du fichier
	ifstream file(filePath);	// pas besoin de gerer l'acces
								// mais il faut s'assurer que le fichier existe
	string content{	std::istreambuf_iterator<char>(file),
					std::istreambuf_iterator<char>() };

	//---------------------------------------------
	// le sha
	// https://pragmaticjoe.blogspot.com/2015/02/how-to-generate-sha1-hash-in-c.html
	sha1 sha;
	sha.process_bytes(content.c_str(), content.length());

	// bof... would prefer array...but the digest does not support it
	unsigned int hash[5];
	sha.get_digest(hash);

	std::stringstream stream;

	std::string result;
	// there is something missing here...but what?
	for (int i = 0; i < 5; ++i) {
		stream << std::hex << hash[i]; 
	}
	content += stream.str();

	//---------------------------------------------------------------
	// poutine syntaxique pour cr�er une compression en zlib
	// https://www.boost.org/doc/libs/1_71_0/libs/iostreams/doc/classes/zlib.html
	boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
	in.push(boost::iostreams::zlib_compressor());
	in.push(boost::make_iterator_range(content));
	std::string compressed;
	boost::iostreams::copy(in, boost::iostreams::back_inserter(compressed));

	//--------------------------------------------------------
	// filesystem
	const auto fileName = "testResultat.txt";

	auto path = boost::filesystem::current_path();	// ou sommes nous?
	const auto pathFolder = path.append("folder");	// ajouter un fichier de la mani�re d�cid�e par la plateforme
													// comme ca, on aura jamais de probl�me de plateforme
	
	if (!boost::filesystem::exists(pathFolder))  // peut lancer une exception
	{
		// si le r�pertoire n'existe pas, on le cr�e
		boost::filesystem::create_directory(pathFolder); // exception possible
	}

	boost::system::error_code code;
	if (!boost::filesystem::exists(pathFolder, code)) // ne lance pas d'exception
	{
		if (code.failed())
		{
			std::cout << "Something bad happened but we didn't throw anything...so thats good :)" << std::endl;
		}
	}


	//---------------------------------------------------------
	// ofstream tout normal et gentil
	std::ofstream outputFile(path.append(fileName).c_str());

	// on �crit la compression, deux sauts de ligne et le contenu
	outputFile << compressed << std::endl << std::endl << content;

	// il faudrait valider l'�criture ici...

	outputFile.close(); // on aime etre propre alors on ferme le fichier
						// th�oriquement, le RAII devrait le faire pour nous
}

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
        writeFile(indexPath, fileText, true);
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
			std::cout << "lineDelimiterA : " << lineDelimiterA << std::endl;
			std::cout << "lineDelimiterB : " << lineDelimiterB << std::endl;
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