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

    Q_INVOKABLE void beverageSelected(Beverage* beverage); // Called from QML when a beverage is selected, updates the weights

    // For QML access
    QString name() const;
    int picture() const;
    QQmlListProperty<Beverage> beverages();

signals:
    void beveragesChanged(); // Emitted when the beverage list changes, to notify QML

private:
    inline static constexpr float conservative_r = 0.05f; // Weight update rate for conservative users
    inline static constexpr float early_adopter_r = 0.20f; // Weight update rate for early adopters

    float m_r = 0.1f; // Weight update rate based on user category

    QString m_name;
    int m_picture = 0;
    UserCategory m_category = UserCategory::Default;

    std::vector<Beverage*> m_beverages; // The list of beverages of the user (with his potential ingredient modifications)
    QVector<float> m_beverages_w; // Weights for each beverage, for recommendation purposes
};

#endif