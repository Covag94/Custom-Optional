#include <iostream>
#include "../include/optional.hpp"

int main()
{
    Optional<int> mpampis;
    mpampis.emplace(42);

    Optional<std::string> kostas("Geia sou kosti");

    Optional<std::string> dimitris;
    dimitris = kostas;

    Optional<std::string> maria = dimitris;
}