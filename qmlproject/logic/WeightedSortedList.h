#ifndef WEIGHTEDSORTEDLIST_H
#define WEIGHTEDSORTEDLIST_H

#include <vector>
#include <algorithm>
#include <cmath>

namespace Logic
{
    /**
     * @class WeightedSortedList
     * @brief A generic container that maintains items sorted by a dynamic weight.
     *
     * This template class manages a fixed set of items (e.g., beverages) paired with
     * normalized weights. It implements an adaptive learning algorithm based on
     * **Exponential Decay**:
     * - When an item is selected, its weight increases significantly.
     * - All other items' weights decrease proportionally to keep the sum = 1.0.
     * - The list automatically reorders itself to keep the highest-weighted items at the top.
     *
     * @tparam T The type of item stored (`Beverage*`).
     */
    template <typename T>
    class WeightedSortedList
    {
    public:
        /**
         * @brief Constructs the list with initial items and uniform weights.
         *
         * @param initialItems The vector of items to manage. The size is fixed after construction.
         * @param weightR The learning rate (0.0 to 1.0). Higher values mean faster adaptation (more reactive).
         */
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

        /**
         * @return The number of items in the list.
         */
        int size() const { return static_cast<int>(m_items.size()); }

        /**
         * @return Const reference to the items vector, sorted by weight (descending).
         */
        const std::vector<T> &items() const { return m_items; }

        /**
         * @return Const reference to the weights vector, parallel to `items()`.
         */
        const std::vector<float> &weights() const { return m_weights; }

        /**
         * @brief Finds the index of a specific item.
         * @param item The item to search for.
         * @return The index (0 to size-1) if found, or -1 if not present.
         */
        int indexOf(const T &item) const
        {
            auto it = std::find(m_items.begin(), m_items.end(), item);
            if (it != m_items.end())
            {
                return static_cast<int>(std::distance(m_items.begin(), it));
            }
            return -1;
        }

        /**
         * @brief Retrieves the current weight of a specific item.
         * @param item The item to query.
         * @return The normalized weight (0.0 to 1.0), or 0.0 if not found.
         */
        float weightFor(const T &item) const
        {
            int idx = indexOf(item);
            return (idx >= 0 && idx < static_cast<int>(m_weights.size())) ? m_weights[idx] : 0.0f;
        }

        /**
         * @brief Retrieves the weight at a specific index.
         * @param index The position in the sorted list.
         * @return The normalized weight, or 0.0 if out of bounds.
         */
        float weightAt(int index) const
        {
            if (index < 0 || index >= static_cast<int>(m_weights.size()))
                return 0.0f;
            return m_weights[index];
        }

        /**
         * @brief Records a selection by index, updating weights and sorting.
         *
         * This applies the exponential decay formula and performs an insertion sort
         * to move the selected item up the ranking if its new weight exceeds its neighbors.
         *
         * @param index The index of the selected item.
         */
        void recordSelectionAt(int index)
        {
            if (index < 0 || index >= static_cast<int>(m_items.size()))
                return;
            applyExponentialDecay(index);
            insertSortUp(index);
        }

        /**
         * @brief Records a selection by item value.
         * Convenience wrapper for `recordSelectionAt`.
         * @param item The item that was selected.
         */
        void recordSelection(const T &item)
        {
            int idx = indexOf(item);
            if (idx >= 0)
                recordSelectionAt(idx);
        }

        /**
         * @brief Sets the learning rate (weight update factor).
         * @param r New rate (clamped between 0.0 and 1.0).
         */
        void setWeightR(float r) { m_weightR = std::clamp(r, 0.0f, 1.0f); }

        /**
         * @return The current learning rate.
         */
        float weightR() const { return m_weightR; }

    private:
        std::vector<T> m_items;  ///< Sorted list of items.
        std::vector<float> m_weights;  ///< Parallel list of normalized weights.
        float m_weightR = 0.175f;  

        /**
         * @brief Internal method to apply mathematical weight updates.
         *
         * Formula for selected item:  W_new = R + (1 - R) * W_old
         * Formula for other items:    W_new = (1 - R) * W_old
         *
         * This ensures the sum of all weights remains 1.0.
         */
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

        /**
         * @brief Internal method to re-sort the list after a weight update.
         *
         * Performs a localized bubble-up (insertion sort) since only one item's weight
         * has increased, meaning it can only move "up" the list (towards index 0).
         * This is O(N) efficient compared to a full sort O(N log N).
         */
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
