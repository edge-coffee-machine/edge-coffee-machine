# target
qul_add_target(edge_coffe_machine QML_PROJECT edge_coffe_machine.qmlproject GENERATE_ENTRYPOINT)

# include of logic folder
target_include_directories(edge_coffe_machine PRIVATE
    logic
)

# header files in logic folder
target_sources(edge_coffe_machine PRIVATE
    logic/Beverage.h
    logic/Beverage.cpp
    logic/BeverageListModel.h
    logic/EdgeCoffeeMachine.h
    logic/EdgeCoffeeMachine.cpp
    logic/IngredientInfo.h
    logic/RecipeDatabase.h
    logic/RecipeDatabase.cpp
    logic/User.h
    logic/User.cpp
    logic/UserListModel.h
    logic/WeightedSortedList.h
)

target_link_libraries(edge_coffe_machine PRIVATE
    Qul::Core
    Qul::Controls
)

# OS specific setup
app_target_setup_os(edge_coffe_machine)
