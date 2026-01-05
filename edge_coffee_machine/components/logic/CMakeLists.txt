idf_component_register(SRCS "Beverage.cpp"
                            "EdgeCoffeeMachine.cpp"
                            "RecipeDatabase.cpp"
                            "User.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES Qul
                       PRIV_REQUIRES face_manager)