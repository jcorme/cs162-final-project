#ifndef ESCAPEFROMCS162_OPTION_H
#define ESCAPEFROMCS162_OPTION_H
/*********************************************************************
** Program Filename: Option.h
** Author: Jason Chen
** Date: 03/05/2018
** Description: Declares the Option class and its related members.
** Input: None
** Output: None
*********************************************************************/


#include <exception>
#include <functional>
#include <iostream>

// This is the exception thrown when an Option that is None or whose value has
// been moved is unwrapped.
class BadOptionAccess : public std::exception {
  public:
    explicit BadOptionAccess(const char* what): what_(what) {}

    const char* what() const noexcept override {
      return what_;
    }

  private:
    const char* what_;
};

// Allows easy construction of empty Option objects (implicitly converts to an
// empty Option).
struct None{};
constexpr None None{};

// Option<T> represents an optional value (a value that may or may not exist).
// This type is modeled after Rust's Option<T>. Option<T> can serve as a much
// safer and more elegant alternative to dealing with and throwing exceptions.
template <typename T>
class Option {
  public:
    Option(): has_value_(false), moved_(false), stub_(nullptr) {}
    Option(const T& t): has_value_(true), moved_(false), value_(t) {}
    Option(T&& t): has_value_(true), moved_(false), value_(std::move(t)) {}
    Option(const Option<T>& o);
    Option(Option<T>&& t) noexcept;
    Option(const struct None&): Option() {}

    ~Option();

    // None means the Option does not hold a value; conversely, Some means
    // the Option does hold a value.
    bool IsNone() const { return !has_value_ || moved_; }
    bool IsSome() const { return has_value_ && !moved_; }

    explicit operator bool() const { return IsSome(); }

    inline const T& CUnwrapRef() const;
    inline const T& CUnwrapRefOr(const T& t) const;
    inline T Unwrap();
    inline T UnwrapOr(const T& t);
    inline T& UnwrapRef();
    inline T& UnwrapRefOr(T& t);

    template <typename E>
    Option<E> AndThen(std::function<Option<E>(T)> f);

    template <typename E>
    Option<E> Map(std::function<E(T)> f);

    template <typename E>
    Option<E> MapCRef(std::function<E(const T&)> f) const;

    Option<T>& operator=(const Option<T>& rhs);
    Option<T>& operator=(Option<T>&& rhs);

  private:
    bool has_value_;
    bool moved_;

    union {
      void* stub_;
      T value_;
    };

    inline void ThrowIfCannotUnwrap() const;
};

/*********************************************************************
** Function: Option
** Description: Copy constructor for the Option class template.
** Parameters: t is the Option<T> being copied.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename T>
Option<T>::Option(const Option<T>& o):
    has_value_(o.has_value_), moved_(o.moved_), stub_(nullptr) {
  if (o.IsSome())
    // Must use placement new here.
    new (&value_) T(o.value_);
}

/*********************************************************************
** Function: Option
** Description: Move constructor for the Option class template.
** Parameters: t is an rvalue reference (to the Option<T> being moved).
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename T>
Option<T>::Option(Option<T>&& t) noexcept:
    has_value_(std::move(t.has_value_)), moved_(std::move(t.moved_)),
    stub_(nullptr) {
  if (t.IsSome())
    new (&value_) T(std::move(t.value_));
}

/*********************************************************************
** Function: ~Option
** Description: Destructor for the Option class template.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename T>
Option<T>::~Option() {
  if (IsSome())
    // With a tagged union, have to call destructor manually (since the compiler
    // has no idea which union member is active at any given time). This is safe
    // for trivial types.
    value_.~T();
}

/*********************************************************************
** Function: CUnwrapRef
** Description: Returns a const reference to the value inside the optional,
 * provided it exists. This function will throw an exception if the value
 * has been moved or doesn't exist.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename T>
const T& Option<T>::CUnwrapRef() const {
  ThrowIfCannotUnwrap();
  return value_;
}

/*********************************************************************
** Function: CUnwrapRefOr
** Description: Returns a reference to the value inside the optional if
 * it exists; otherwise, returns the argument.
** Parameters: t is the default value to return if the optional is None.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename T>
const T& Option<T>::CUnwrapRefOr(const T& t) const {
  if (IsNone()) return t;
  return CUnwrapRef();
}

/*********************************************************************
** Function: Unwrap
** Description: Moves (returns) the value held by the optional if it exists;
 * this function will throw an exception if the value has already been
 * moved or no value exists.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename T>
T Option<T>::Unwrap() {
  ThrowIfCannotUnwrap();
  moved_ = true;
  return std::move(value_);
}

/*********************************************************************
** Function: UnwrapOr
** Description: Moves (returns) the value held by the optional if it exists;
 * otherwise, returns the argument.
** Parameters: t is the default value to return if the optional is None.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename T>
T Option<T>::UnwrapOr(const T& t) {
  if (IsNone()) return t;
  return Unwrap();
}

/*********************************************************************
** Function: UnwrapRef
** Description: Returns a reference to the value inside the optional,
 * provided it exists. This function will throw an exception if the value
 * has been moved or doesn't exist.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename T>
T& Option<T>::UnwrapRef() {
  ThrowIfCannotUnwrap();
  return value_;
}

/*********************************************************************
** Function: UnwrapRefOr
** Description: Returns a reference to the value inside the optional if
 * it exists; otherwise, returns the argument.
** Parameters: t is the default value to return if the optional is None.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename T>
T& Option<T>::UnwrapRefOr(T& t) {
  if (IsNone()) return t;
  return UnwrapRef();
}

/*********************************************************************
** Function: AndThen
** Description: Transforms an Option<T> to an Option<E> by using the given
 * function (T -> Option<E>) to transform a T into Option<E>; returns None
 * if no value exists; moves the value if it does exist.
** Parameters: f is a function (T -> Option<E>).
** Pre-Conditions:
** Post-Conditions:
*********************************************************************/
template <typename T>
template <typename E>
Option<E> Option<T>::AndThen(std::function<Option<E>(T)> f) {
  if (IsNone()) return None;
  moved_ = true;
  return f(std::move(value_));
}

/*********************************************************************
** Function: Map
** Description: Transforms an Option<T> to an Option<E> by using the given
 * function (T -> E) to map T to E; returns None if no value exists; moves
 * the value if it does exist.
** Parameters: f is a function (T -> E).
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename T>
template <typename E>
Option<E> Option<T>::Map(std::function<E(T)> f) {
  if (IsNone()) return None;
  moved_ = true;
  return f(std::move(value_));
}

/*********************************************************************
** Function: MapCRef
** Description: Transforms an Option<T> to an Option<E> by using the given
 * function (T -> E) to map T to E; returns None if no value exists; does not
 * move the value.
** Parameters: f is a function (T -> E).
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename T>
template <typename E>
Option<E> Option<T>::MapCRef(std::function<E(const T&)> f) const {
  if (IsNone()) return None;
  return f(value_);
}

/*********************************************************************
** Function: operator=
** Description: Overloads the assignment operator to implement safe copying
 * of Option classes.
** Parameters: rhs is the Option<T> being copied.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename T>
Option<T>& Option<T>::operator=(const Option<T>& rhs) {
  if (IsSome()) value_.~T();

  has_value_ = rhs.has_value_;
  moved_ = rhs.moved_;

  if (rhs.IsSome())
    new (&value_) T(rhs.value_);
  else
    stub_ = nullptr;

  return *this;
}

/*********************************************************************
** Function: operator=
** Description: Overloads the assignment operator to implement safe moving
 * of Option classes.
** Parameters: rhs is the Option<T> being moved.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename T>
Option<T>& Option<T>::operator=(Option<T>&& rhs) {
  if (this != &rhs) {
    if (IsSome()) value_.~T();

    has_value_ = std::move(rhs.has_value_);
    moved_ = std::move(rhs.moved_);

    if (rhs.IsSome())
      new (&value_) T(std::move(rhs.value_));
    else
      stub_ = nullptr;
  }

  return *this;
}

/*********************************************************************
** Function: ThrowIfCannotUnwrap
** Description: Throws an exception if called and the value has been moved
 * or no value exists.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename T>
void Option<T>::ThrowIfCannotUnwrap() const {
  if (moved_)
    throw BadOptionAccess("Cannot unwrap a moved value.");

  if (!has_value_)
    throw BadOptionAccess("Cannot unwrap an empty Option.");
}

/*********************************************************************
** Function: operator<<
** Description: Overloads the insertion operator to print Option<T> values.
** Parameters: os is the output_stream; possible_t is the Option<T> to print.
** Pre-Conditions:  None
** Post-Conditions: None
*********************************************************************/
template <typename T>
std::ostream& operator<<(std::ostream& os, const Option<T>& o) {
  if (o.IsNone()) return os << "None";
  return os << "Some(" << o.CUnwrapRef() << ')';
}


#endif //ESCAPEFROMCS162_OPTION_H
