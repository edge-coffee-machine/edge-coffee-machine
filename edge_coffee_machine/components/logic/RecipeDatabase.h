#ifndef RECIPEDATABASE_H
#define RECIPEDATABASE_H

#include <vector>
#include "Beverage.h"

namespace Logic
{
    /**
     * @class RecipeDatabase
     * @brief A static factory class acting as the repository for beverage recipes.
     *
     * This class encapsulates the creation logic for all standard beverages supported
     * by the machine (Espresso, Cappuccino, etc.). It serves as the "Single Source
     * of Truth" for the factory-default settings (ingredients, limits, and defaults).
     *
     * It is designed to be stateless and used purely to generate fresh instances
     * of beverages when needed.
     */
    class RecipeDatabase
    {
    public:
        /**
         * @brief Creates and returns the list of all default beverages.
         *
         * Allocates new `Beverage` instances on the heap with pre-configured
         * ingredient values (Min/Max/Default) corresponding to standard recipes.
         *
         * @return A `std::vector` containing pointers to the newly created `Beverage` objects.
         */
        static std::vector<Beverage *> getAllDefaultRecipes();
    };
}

#endif // RECIPEDATABASE_H