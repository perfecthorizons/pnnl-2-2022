#define MAX_DEPTH 7

template<int N>
struct flag {
  friend constexpr int adl_flag(flag<N>);
};

template<int N> struct depth {};

template<int N>
struct mark {
  friend constexpr int adl_flag(flag<N>) {
    return N;
  }

  static constexpr int value = N;
};

template<int D, int N>
int constexpr binary_search_flag(float, depth<D>, flag<N>, int next_flag = binary_search_flag(0, depth<D-1>(), flag<N - (1 << (D - 1))>())) {
  return next_flag;
}

template <int N, class = char[noexcept( adl_flag(flag<N>()) ) ? +1 : -1]>
int constexpr binary_search_flag(int,   depth<0>, flag<N>) {
    return N + 1;
}

template <int N>
int constexpr binary_search_flag(float, depth<0>, flag<N>) {
    return N;
}

template<int next_flag = binary_search_flag(0, depth<MAX_DEPTH-1>(), flag<(1 << (MAX_DEPTH-1))>())>
int constexpr __id__(int value = mark<next_flag>::value) {
    return value;
}
