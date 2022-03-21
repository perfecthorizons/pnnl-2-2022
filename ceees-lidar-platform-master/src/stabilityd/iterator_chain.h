#pragma once

// TODO: figure out cend stuff

template<typename T>
class IteratorChainBase {
public:
    IteratorChainBase &operator++();
    bool operator==(const IteratorChainBase<T> &other);
    bool operator==(const std::iterator<T> &other);
    bool operator!=(const IteratorChainBase<T> &other);
    bool operator!=(const std::iterator<T> &other);
    T &operator*();
    T *operator->();

    std::iterator<T> cend;
};

template<typename T, typename... Iters>
class IteratorChain : public IteratorChainBase<T> {
private:
    static_assert(sizeof...(Iters) % 2 == 0, "Iterators must be passed in pairs");

public:
    IteratorChain(Iters... its) : _idx(0), _it_pairs(std::make_index_sequence<sizeof...(Iters)/2>{}, std::make_tuple(its...)) {
        it = _it_pairs[0][0];
    };

private:
    template<size_t... Is, class Tuple>
    idx_pairs(std::index_sequence<Is...>, Tuple tuple) {
        return std::vector<std::pair<size_t, size_t>> {
            std::make_pair(std::get<2*Is>(tuple), std::get<2*Is+1>(tuple))...
        };
    }

    size_t _idx;

public:
    IteratorChain &operator++() {
        ++it;
        if (it == _it_pairs[_idx][1]) {
            ++_idx;
            if (_idx == sizeof...(Iters)/2) {
                it=end;
            } else {
                it=_it_pairs[_idx][0];
            }
        }
    }

    bool operator==(const IteratorChain<> &other) {
        return it == other.it;
    };

    bool operator!=(const IteratorChain &other) {
        return it != other.it;
    };

    bool operator==(const std::iterator<T> &other) {
        return it == other;
    };

    bool operator!=(const std::iterator<T> &other) {
        return it != other;
    };

    T &operator*() {
        return *it;
    };

    T *operator->() {
        return &(operator*());
    };

private:
    std::vector<std::pair<std::iterator<T>, std::iterator<T>>> _it_pairs;
    std::iterator<T> it;
};