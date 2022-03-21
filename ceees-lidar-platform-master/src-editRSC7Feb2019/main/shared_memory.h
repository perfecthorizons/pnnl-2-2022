#ifndef _SHARED_MEMORY_T
#define _SHARED_MEMORY_T

#include <shared_mutex>
#include <tuple>
#include <initializer_list>
#include "debug.h"

/*******************************************************************************
all and are_all_convertible are two variadic templates used with the name method
to determine if the names are all types convertible to string
*******************************************************************************/
template<bool...> struct boolean_pack;
template<bool... bools>
using all = std::is_same<
  boolean_pack<bools..., true>,
  boolean_pack<true, bools...>
>;

template<class R, class... StringType>
using are_all_convertible = all<std::is_convertible<StringType, R>::value...>;


/*******************************************************************************
generate_tuple is a variadic template used to generate arbitrary-length
homogenous tuples
*******************************************************************************/
template<typename T, size_t N, typename... Types>
struct generate_tuple {
 typedef typename generate_tuple<T, N-1, T, Types...>::type type;
};

template<typename T, typename... Types>
struct generate_tuple<T, 1, Types...> {
  typedef std::tuple<Types...> type;
};

/************************************************************** shared_memory */
template<typename... Types>
class shared_memory {
  using string_tuple_t = typename generate_tuple<std::string, sizeof...(Types)>::type;
public:

  /******************************************************* shared_memory data */
  std::tuple<Types...> m;
  std::shared_mutex *lk;
private:
  string_tuple_t _names;
  bool _is_named;
public:

  /************************************************ shared_memory constructor */
  shared_memory() : m(), _is_named(false) {
    LOG(4, "Created new shared memory");
    lk = new std::shared_mutex();
  };
  /************************************************* shared_memory destructor */
  ~shared_memory() {
    LOG(4, "Deleting shared memory");
    delete lk;
  }

/******************************************************** shared_memory::name */
  //TODO: fix later
 //template<
 // typename... StringTypes,
 // typename = typename std::enable_if<
 // are_all_convertible<const char[], StringTypes...>::value &&
 // sizeof...(StringTypes) == sizeof...(Types)
 // >::type
 //> 
  template<typename... StringTypes>
  void name(StringTypes... strings) {
    _is_named = true;
    _names = std::make_tuple(strings...);
  }

  /************************************************** shared_memory::is_named */
  bool is_named() { return _is_named; };

  /***************************************************** shared_memory::names */
  string_tuple_t names() {
    if (!_is_named) throw "Error: shared memory is unnamed.";
    return _names;
  }

  /***************************************************** shared_memory::names */
  std::string names(std::string glue, std::index_sequence<sizeof...(Types)>) {
    if (!_is_named) throw "Error: shared memory is unnamed.";
    return __join_tuple(_names, glue, std::make_index_sequence<sizeof...(Types)>());
  }
private:
  /********************************************** shared_memory::__join_tuple */
  template<class TupleType, size_t... I>
  std::string __join_tuple(TupleType &tuple, std::string glue, std::index_sequence<I...>) {
    std::string output;
    (..., (output += std::to_string(std::get<I>(tuple)) + glue));
    return output.substr(0, output.length() - 2);
  }
public:

  /****************************************************** shared_memory::read */
  template<std::size_t I>
  typename std::tuple_element<I, std::tuple<Types...>>::type read() {
    //std::shared_lock<std::shared_mutex> guard(*lk);
    //LOG(3, "Reading from shared memory (element " << I << ")");
    return std::get<I>(m);
  }

  /****************************************************** shared_memory::read */
  std::tuple<Types...> read() {
    //std::shared_lock<std::shared_mutex> guard(*lk);
    //LOG(3, "Reading from shared memory");
    return m;
  }

  /****************************************************** shared_memory::read */
  std::string read(std::string glue) {
    std::shared_lock<std::shared_mutex> guard(*lk);
    //LOG(3, "Reading from shared memory");
    return __join_tuple(m, glue, std::make_index_sequence<sizeof...(Types)>());
  }

  /***************************************************** shared_memory::write */
  template<std::size_t I>
  void write(typename std::tuple_element<I, std::tuple<Types...>>::type new_m) {
    //std::unique_lock<std::shared_mutex> guard(*lk);
    LOG(3, "Writing to shared memory (element " << I << ")");
    std::get<I>(m) = new_m;
  }

  /***************************************************** shared_memory::write */
  void write(std::tuple<Types...> new_m) {
    //std::unique_lock<std::shared_mutex> guard(*lk);
    //LOG(3, "Writing to shared memory");
    m = new_m;
  }

  /***************************************************** shared_memory::write */
  void write(Types... new_m) {
    //std::unique_lock<std::shared_mutex> guard(*lk);
    //LOG(3, "Writing to shared memory");
    m = std::make_tuple(new_m...);
  }

};

#endif
