#ifndef WEIGHTEDSORTEDLIST_H
#define WEIGHTEDSORTEDLIST_H

#include <vector>
#include <algorithm>
#include <cmath>

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

namespace Logic
{
    template <typename T>
    class WeightedSortedList
    {
    public:
        explicit WeightedSortedList(const std::vector<T> &initialItems = {}, float weightR = 0.175f)
            : m_items(initialItems), m_weightR(weightR)
        {
            int n = m_items.size();
            if (n > 0)
            {
                float w = 1.0f / static_cast<float>(n);
                m_weights = std::vector<float>(n, w);
            }
        }

        // Basic queries
        int size() const { return static_cast<int>(m_items.size()); }
        const std::vector<T> &items() const { return m_items; }         // items ordered by weight (desc)
        const std::vector<float> &weights() const { return m_weights; } // parallel normalized weights

        int indexOf(const T &item) const
        {
            auto it = std::find(m_items.begin(), m_items.end(), item);
            if (it != m_items.end())
            {
                return static_cast<int>(std::distance(m_items.begin(), it));
            }
            return -1;
        }

        // Get weight by item
        float weightFor(const T &item) const
        {
            int idx = indexOf(item);
            return (idx >= 0 && idx < static_cast<int>(m_weights.size())) ? m_weights[idx] : 0.0f;
        }

        // Get weight by index
        float weightAt(int index) const
        {
            if (index < 0 || index >= static_cast<int>(m_weights.size()))
                return 0.0f;
            return m_weights[index];
        }

        // Record selection by index
        void recordSelectionAt(int index)
        {
            if (index < 0 || index >= static_cast<int>(m_items.size()))
                return;
            applyExponentialDecay(index);
            insertSortUp(index);
        }

        // Record selection by item
        void recordSelection(const T &item)
        {
            int idx = indexOf(item);
            if (idx >= 0)
                recordSelectionAt(idx);
        }

        // Configuration
        void setWeightR(float r) { m_weightR = std::clamp(r, 0.0f, 1.0f); }
        float weightR() const { return m_weightR; }

    private:
        std::vector<T> m_items;
        std::vector<float> m_weights; // normalized, parallel to m_items
        float m_weightR = 0.175f;

        // Apply exponential decay weight update
        void applyExponentialDecay(int selectedIdx)
        {
            for (int i = 0; i < m_weights.size(); ++i)
            {
                if (static_cast<int>(i) == selectedIdx)
                {
                    m_weights[i] = m_weightR + (1.0f - m_weightR) * m_weights[i];
                }
                else
                {
                    m_weights[i] = (1.0f - m_weightR) * m_weights[i];
                }
            }
        }

        // Insert-sort the selected item upward based on updated weights
        void insertSortUp(int selectedIdx)
        {
            int i = selectedIdx;
            while (i > 0 && m_weights[i] > m_weights[i - 1])
            {
                std::swap(m_items[i], m_items[i - 1]);
                std::swap(m_weights[i], m_weights[i - 1]);
                --i;
            }
        }
    };
}
#endif // WEIGHTEDSORTEDLIST_H
