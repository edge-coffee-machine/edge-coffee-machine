#ifndef RECIPEDATABASE_H
#define RECIPEDATABASE_H

#include <vector>
#include "Beverage.h"

namespace Logic
{
    class RecipeDatabase
    {
    public:
        static std::vector<Beverage *> getAllDefaultRecipes();
    };

}

#endif