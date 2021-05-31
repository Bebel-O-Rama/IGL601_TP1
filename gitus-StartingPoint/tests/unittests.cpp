#define CATCH_CONFIG_MAIN

// RTFM catch2:
// https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#top

#include "catch.hpp"
#include <utilitary.h>
#include "../fonction.cpp"



TEST_CASE("Test de Init") 
{ 
    if (pathExists(GIT_PATH)) 
    { 
        boost::filesystem::remove_all(GIT_PATH);
    } 
    REQUIRE(init() == true);
    REQUIRE(pathExists(GIT_PATH) == true); 
    REQUIRE(pathExists(GIT_PATH + "objects") == true); 
    REQUIRE(fileExists(GIT_PATH + "HEAD")); 
    REQUIRE(fileExists(GIT_PATH + "index")); 

    //On regarde ce qui est dans les fichier pour voir si ils sont vides
    std::string index = readFile(GIT_PATH + "index"); 
    std::string head = readFile(GIT_PATH + "HEAD"); 
    REQUIRE(index == ""); 
    REQUIRE(head == ""); 
}

TEST_CASE("Test de Add") 
{ 
    writeFile(GIT_PATH + "a.txt","jazzband");
    std::string path = "a.txt";
    //On fait un add en s<assurant que celui-ci se finit correctement
    REQUIRE(add(GIT_PATH + path) == true);

    //on lit les fichier pour verifier leur contenu
    std::string object = readFile((GIT_PATH + "objects/81/fd2e63da02b053e51e3465ddcdf8b5bade3a0"));
    std::string index = readFile((GIT_PATH + "index"));
    //Ces string represente le nom du fichier, sa taille et ce qu<il contient
    
    std::string length = "8";
    std::string text = "jazzband";
    std::string sha = "81fd2e63da02b053e51e3465ddcdf8b5bade3a0";
    std::string object_test = (GIT_PATH + path + "\n\n" + length + "\n\n" + text);
    std::string index_test = sha + " " + GIT_PATH + path + "\n";

    

    
    //On test ici si les fichier object on ete cree
    REQUIRE(pathExists(GIT_PATH + "objects/81") == true); 
    REQUIRE(fileExists(GIT_PATH + "objects/81/fd2e63da02b053e51e3465ddcdf8b5bade3a0")); 
    //On test ici le contenu des fichiers object et index
    REQUIRE(object == object_test); 
    REQUIRE(index == index_test);
    //On test ici si add retourne bien false si le path vers le fichier est eronne
    REQUIRE(add(path) == false);

    
}

TEST_CASE("Test de Commit") 
{
    //Test le cas ou le message est vide
    REQUIRE(commit("","yo") == false);
    //Test le cas ou l<auteur est vide
    REQUIRE(commit("yo","") == false);

    //test du premier commit
    REQUIRE(commit("Premier commit","moi") == true);

    //test si l<index est vide
    REQUIRE(commit("genre","moi2") == false);
    //On modifie le fichier et on l<add pour pouvoir le commit a nouveau
    writeFile(GIT_PATH + "a.txt","jazzband");
    REQUIRE(add(GIT_PATH + "a.txt") == true);
    //test deuxieme commit
    REQUIRE(commit("Deuxieme commit","moi") == true);

    //On lit ce qui est dans le head pour verifier que le fichier ou il doit pointer existe
    std::string head = readFile((GIT_PATH + "HEAD"));
    std::string head1 = head.substr(0,2);
    std::string head2 = head.substr(2,head.length()-1);
    std::cout<<head<<std::endl<<head1<<std::endl<<head2<<std::endl;
    REQUIRE(fileExists(GIT_PATH + "objects/" + head1 + "/" + head2)); 
}

