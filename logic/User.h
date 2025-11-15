#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>
#include <QQmlListProperty>
#include <vector>
#include <QVector>
#include "Beverage.h"

/*
 * User
 *
 * Purpose
 * -------
 * Represents a person of the coffee machine application, exposing user identity and
 * personalized beverage preferences to QML and the recommendation subsystem.
 * The class encapsulates a small recommendation model that updates per-beverage
 * weights and user behaviour scores (tryer / customizer) whenever the user
 * selects (brews) or customizes a beverage from the UI.
 * 
 * Implementation notes
 * --------------------
 * - The beverages method exposes m_beverages to QML as a QQmlListProperty. m_beverages is always
 *   sorted by weight after updates to ensure the UI reflects current recommendations.
 * - m_beverages and m_beveragesW are kept in sync via parallel updates and sorting.
 *
 * Recommendation & Classification Model (high level)
 * -------------------------------------------------
 * - Per-beverage weights are updated using an exponential-style decay model:
 *     - The selected beverage receives a positive update (increased weight).
 *     - Other beverages are decayed accordingly so the vector remains meaningful
 *       for ranking and recommendation.
 * - The per-user weight update rate m_weightR is driven by the user's current
 *   category (Default, Conservative, EarlyAdopter). Category-specific constants
 *   (conservativeWeightR, earlyAdopterWeightR) determine how aggressively weights
 *   move after a selection.
 * - Two behavioural scores are tracked:
 *     - m_tryerScore: how often the user tries new beverages
 *     - m_customizerScore: how often the user customizes beverages before selecting
 *   These are updated when selections occur.
 * - classifyUser() combines tryer and customizer scores (using tryerToCustomizerRatio)
 *   and a threshold (earlyAdopterTreshold) to promote users into the EarlyAdopter
 *   category. Classification affects future weight update rates.
 */

class User : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT) // User's name. Set only at creation
    Q_PROPERTY(int picture READ picture CONSTANT) // User's picture index. Set only at creation
    Q_PROPERTY(QQmlListProperty<Beverage> displayBeverages READ displayBeverages) // Beverages sorted by recommendation (for early adopters, an unfrequent beverage appears third)


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

    /*
    Called from QML when a beverage is customized
    beverage: pointer to the beverage customized by the user from QML

    Records that the user has customized a beverage for recommendation purposes.
    For the actual change in ingredients, use the Beverage methods directly.
    */
    Q_INVOKABLE void beverageCustomized();

    void test();

    // For QML access
    // Gets the user's name
    QString name() const;

    // Gets the user's picture index
    int picture() const;

    /*
    Gets the list of user's beverages to display for QML access.
    For conservative users, the list is sorted by recommendation weight, so that 
    more recommended beverages appear earlier in the list.
    For early adopters, the third beverage in the list is always an unfrequent beverage.
    */
    QQmlListProperty<Beverage> displayBeverages();

signals:
    void displayBeveragesChanged(); // Emitted when the beverage list changes, to notify QML

private:
    /*
    Should only be called from beverageSelected.
    Updates the weights after a beverage has been selected.
    selectedIdx: index of the beverage that has been selected in m_beverages (whose weight will be increased)
    */
    void updateBeverageWeights(int selectedIdx);

    /*
    Should only be called from beverageSelected.
    Updates the user scores and category based on the selected beverage.
    Resets m_customized flag.
    selectedIdx: index of the beverage that has been selected in m_beverages
    */
    void classifyUser(int selectedIdx);

    /*
    Should only be called from beverageSelected.
    Called each time the weights change, to sort the modified beverage (and weight).
    It does NOT do a full sort, only repositions the selectedIdx item as needed to maintain order.
    Higher weight beverages will be earlier in the list.
    Maintains the correspondence between m_beverages and m_beveragesW.
    */
    void reorderBeverage(int selectedIdx);

    /*
    Updates m_displayBeverages based on the current user category and beverage list.
    */
    void updateDisplayBeverages();

    inline static constexpr float conservativeWeightR = 0.1f; // Weight update rate for conservative users
    inline static constexpr float earlyAdopterWeightR = 0.25f; // Weight update rate for early adopters
    inline static constexpr float tryerR = 0.25f; // Tryer score update rate
    inline static constexpr float customizerR = 0.225f; // Customizer score update rate
    inline static constexpr float tryerToCustomizerRatio = 0.8f; // How much more important is the tryer score vs the customizer score in classifying users
    inline static constexpr float earlyAdopterTreshold = 0.5f; // Treshold for early adopter classification

    // Basic properties
    QString m_name;
    int m_picture = 0;
    UserCategory m_category = UserCategory::Default;
    std::vector<Beverage*> m_beverages; // The sorted list of beverages of the user, with his potential ingredient modifications

    // Recomendation system data/parameters
    int m_numBeverages = 0; // Number of beverages the user has selected
    std::vector<float> m_beveragesW; // Weights for each beverage, for recommendation purposes. (Should be) synced with m_beverages
    std::vector<Beverage*> m_displayBeverages;
    float m_tryerScore = 0.0f; // Score for how much the user tries new beverages
    float m_customizerScore = 0.0f; // Score for how much the user customizes beverages

    float m_weightR = 0.175f; // Update rate for the coffee weights, based on user category
    bool m_customized = false; // Wether the user has customized a drink since the last beverage selection
};

#endif