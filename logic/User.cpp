#include "User.h"
#include <QDebug>

// Constructor, receives user name, picture index and optional parent QObject
User::User(const QString& name, int picture, QObject* parent)
    : QObject(parent), m_name(name), m_picture(picture), m_category(UserCategory::Default)
{
    //m_beverages = Beverage::createDefaultTemplates(this);
    m_beverages.clear();
    m_beverages.push_back(new Beverage(QStringLiteral("Espresso"), this));
    m_beverages.push_back(new Beverage(QStringLiteral("Cappuccino"), this));
    m_beverages.push_back(new Beverage(QStringLiteral("Americano"), this));

    float w = 1.0f / static_cast<float>(m_beverages.size());
    m_beveragesW = std::vector<float>(m_beverages.size(), w);
    emit beveragesChanged();
}

QString User::name() const { return m_name; }
int User::picture() const { return m_picture; }

QQmlListProperty<Beverage> User::beverages()
{
    return QQmlListProperty<Beverage>(this, this,
        [](QQmlListProperty<Beverage>* list) -> qsizetype {
            return static_cast<qsizetype>(reinterpret_cast<User*>(list->data)->m_beverages.size());
        },
        [](QQmlListProperty<Beverage>* list, qsizetype index) -> Beverage* {
            return reinterpret_cast<User*>(list->data)->m_beverages.at(static_cast<size_t>(index));
        }
    );
}

void User::beverageSelected(Beverage* beverage)
{
    if (!beverage) return;

    qInfo() << "User::beverageSelected called for beverage:" << beverage->name();

    // Locate the beverage index
    int idx = -1;
    for (int i = 0; i < m_beverages.size(); i++) {
        if (m_beverages[i] == beverage) {
            idx = i;
            break;
        }
    }

    if (idx < 0) {
        qWarning() << "   User::beverageSelected: Beverage not found!";
        return;
    }

    classifyUser(idx);
    updateBeverageWeights(idx);
    sortBeverages();

    m_numBeverages++;
}

void User::beverageCustomized()
{
    qInfo() << "User::beverage_customized called.";
    m_customized = true;
}

void User::updateBeverageWeights(int selectedIdx)
{
    // Update coffee weights
    for (int i = 0; i < m_beveragesW.size(); i++) {
        if (i == selectedIdx) {
            m_beveragesW[i] = m_weightR + (1.0f - m_weightR) * m_beveragesW[i];
        } 
        else {
            m_beveragesW[i] = (1.0f - m_weightR) * m_beveragesW[i];
        }
    }

    // Normalize weights
    float sum = 0.0f;
    for (size_t i = 0; i < m_beveragesW.size(); ++i) {
        sum += m_beveragesW[i];
    }
    if (sum > 0.0f) {
        for (size_t i = 0; i < m_beveragesW.size(); ++i) {
            m_beveragesW[i] /= sum;
        }
    }
}

void User::classifyUser(int selectedIdx){
    // Update tryer score
    m_tryerScore = tryerR*(1-m_beveragesW[selectedIdx]) + (1-tryerR)*(m_tryerScore);

    // Update customizer score
    if (m_customized) {
        m_customizerScore = customizerR + (1-customizerR)*(m_customizerScore);
    }
    else {
        m_customizerScore = (1-customizerR)*(m_customizerScore);
    }

    m_customized = false;

    // print tryer score, customizer score, and beverage weights for debugging
    qInfo() << "   User::beverageSelected: tryerScore =" << m_tryerScore << ", customizerScore =" << m_customizerScore;

    if (m_numBeverages >= 3) { // Classify the user after 3 selections
        // Calculate an early adopter score to classify the user based on the tryer and customizer scores
        float earlyAdopterScore = tryerToCustomizerRatio*m_tryerScore + (1-tryerToCustomizerRatio)*m_customizerScore; 

        qInfo() << "   User::beverageSelected: earlyAdopterScore =" << earlyAdopterScore;
        
        // Classify user
        if (earlyAdopterScore < earlyAdopterTreshold) {
            m_category = UserCategory::Conservative;
            m_weightR = conservativeWeightR;
        }
        else {
            m_category = UserCategory::EarlyAdopter;
            m_weightR = earlyAdopterWeightR;
        }
    }

    QString categoryStr;
    switch (m_category) {
        case UserCategory::Default:
            categoryStr = QStringLiteral("Default");
            break;
        case UserCategory::Conservative:
            categoryStr = QStringLiteral("Conservative");
            break;
        case UserCategory::EarlyAdopter:
            categoryStr = QStringLiteral("Early adopter");
            break;
        default:
            categoryStr = QStringLiteral("Unknown");
            break;
    }

    qInfo() << "   User::beverageSelected: User category =" << categoryStr;
}

void User::sortBeverages()
{
    size_t n = m_beverages.size();
    if (n <= 1) return;

    std::vector<size_t> idxs(n);
    for (size_t i = 0; i < n; ++i) idxs[i] = i;

    std::sort(idxs.begin(), idxs.end(), [&](size_t a, size_t b){
        float wa = (a < m_beveragesW.size()) ? m_beveragesW[a] : 0.0f;
        float wb = (b < m_beveragesW.size()) ? m_beveragesW[b] : 0.0f;
        return wa > wb;
    });

    std::vector<Beverage*> new_bev;
    std::vector<float> new_w;
    new_bev.reserve(n);
    new_w.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        new_bev.push_back(m_beverages[idxs[i]]);
        new_w.push_back((idxs[i] < m_beveragesW.size()) ? m_beveragesW[idxs[i]] : 0.0f);
    }

    m_beverages.swap(new_bev);
    m_beveragesW.swap(new_w);

    qInfo() << "   New beverage weights:";
    for (int i = 0; i < m_beveragesW.size(); i++) {
        qInfo() << "      Beverage" << i << "(" << m_beverages[i]->name() << "): weight =" << m_beveragesW[i];
    }

    emit beveragesChanged();
}