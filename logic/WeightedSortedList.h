#ifndef WEIGHTEDSORTEDLIST_H
#define WEIGHTEDSORTEDLIST_H

#include <QList>
#include <QVector>
#include <algorithm>

/*
 WeightedSortedList<T>
 ---------------------
 Header-only template that keeps a fixed list of items together with a
 parallel vector of normalized weights. Updates use an exponential-decay
 rule and a single insert-sort pass to move the selected item upward.
 - The item list is fixed after construction.
 - Weights are always normalized (sum == 1) after construction and after updates.
 - T is typically a pointer type (e.g. Beverage*).
*/

template<typename T>
class WeightedSortedList {
public:
    explicit WeightedSortedList(const QList<T>& initialItems = {}, float weightR = 0.175f)
        : m_items(initialItems), m_weightR(weightR)
    {
        int n = m_items.size();
        if (n > 0) {
            float w = 1.0f / static_cast<float>(n);
            m_weights = QVector<float>(n, w);
        }
    }

    // Basic queries
    int size() const { return m_items.size(); }
    const QList<T>& items() const { return m_items; }          // items ordered by weight (desc)
    const QVector<float>& weights() const { return m_weights; } // parallel normalized weights
    int indexOf(const T& item) const { return m_items.indexOf(item); }

    float weightFor(const T& item) const {
        int idx = indexOf(item);
        return (idx >= 0 && idx < m_weights.size()) ? m_weights[idx] : 0.0f;
    }

    float weightAt(int index) const {
        if (index < 0 || index >= m_weights.size()) return 0.0f;
        return m_weights[index];
    }

    // Record selection by index or item
    void recordSelectionAt(int index) {
        if (index < 0 || index >= m_items.size()) return;
        applyExponentialDecay(index);
        insertSortUp(index);
    }

    void recordSelection(const T& item) {
        int idx = indexOf(item);
        if (idx >= 0) recordSelectionAt(idx);
    }

    // Configuration
    void setWeightR(float r) { m_weightR = r; }
    float weightR() const { return m_weightR; }

private:
    QList<T> m_items;
    QVector<float> m_weights; // normalized, parallel to m_items
    float m_weightR = 0.175f;

    void applyExponentialDecay(int selectedIdx) {
        for (int i = 0; i < m_weights.size(); ++i) {
            if (i == selectedIdx) {
                m_weights[i] = m_weightR + (1.0f - m_weightR) * m_weights[i];
            } else {
                m_weights[i] = (1.0f - m_weightR) * m_weights[i];
            }
        }
        normalizeWeights();
    }

    void insertSortUp(int selectedIdx) {
        int i = selectedIdx;
        while (i > 0 && m_weights[i] > m_weights[i - 1]) {
            std::swap(m_items[i], m_items[i - 1]);
            std::swap(m_weights[i], m_weights[i - 1]);
            --i;
        }
    }

    // Force normalization if needed
    void normalizeWeights() {
        float sum = 0.0f;
        for (float w : m_weights) sum += w;
        if (sum > 0.0f) {
            for (int i = 0; i < m_weights.size(); ++i) m_weights[i] /= sum;
        }
    }
};

#endif // WEIGHTEDSORTEDLIST_H