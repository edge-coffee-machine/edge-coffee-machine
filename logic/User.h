/*
This class handles everything related to (recognized) users:
- Name
- Picture
- Category (default, conservative, early adopter)
- Beverage list 
- Interaction history
- Creating a personalized menu
*/

#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>
#include <QQmlListProperty>
#include <vector>
#include <QVector>
#include "Beverage.h"

class User : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT) // User's name. Set only at creation
    Q_PROPERTY(int picture READ picture CONSTANT) // User's picture index. Set only at creation
    Q_PROPERTY(QQmlListProperty<Beverage> beverages READ beverages NOTIFY beveragesChanged) // List of user's beverages. Initialized with the defaults

public:
    enum class UserCategory { Default, Conservative, EarlyAdopter };

    explicit User(const QString& name, int picture, QObject* parent = nullptr); // Constructor

    /*
    Called from QML when a beverage is selected
    beverage: pointer to the beverage selected by the user from QML

    Updates the weights of the beverages according to the user's category and selection.
    Follows an exponential decay model where the selected beverage's weight is increased
    and the others are decreased, based on the user's update rate m_r.
    */
    Q_INVOKABLE void beverageSelected(Beverage* beverage); 

    // Called from QML when a beverage is customized, updates the beverage ingredients
    // The ingredients should be in a range of 0.0 to 1.0 representing the proportion of each ingredient

    /*
    Called from QML when a beverage is customized
    beverage: pointer to the beverage customized by the user from QML

    Records that the user has customized a beverage for recommendation purposes.
    For the actual change in ingredients, use the Beverage methods directly.
    */
    Q_INVOKABLE void beverageCustomized();
    // For QML access
    QString name() const;
    int picture() const;
    QQmlListProperty<Beverage> beverages();

signals:
    void beveragesChanged(); // Emitted when the beverage list changes, to notify QML

private:
    inline static constexpr float conservativeWeightR = 0.1f; // Weight update rate for conservative users
    inline static constexpr float earlyAdopterWeightR = 0.25f; // Weight update rate for early adopters
    inline static constexpr float tryerR = 0.25f; // Tryer score update rate
    inline static constexpr float customizerR = 0.225f; // Customizer score update rate

    // Basic properties
    QString m_name;
    int m_picture = 0;
    UserCategory m_category = UserCategory::Default;
    std::vector<Beverage*> m_beverages; // The list of beverages of the user (with his potential ingredient modifications)

    // Recomendation system data/parameters
    int m_numBeverages = 0; // Number of beverages the user has selected
    std::vector<float> m_beveragesW; // Weights for each beverage, for recommendation purposes
    float m_tryerScore = 0.0f; // Score for how much the user tries new beverages
    float m_customizerScore = 0.0f; // Score for how much the user customizes beverages

    float m_weightR = 0.175f; // Update rate for the coffee weights, based on user category
    bool m_customized = false; // Wether the user has customized a drink since the last beverage selection
};

#endif