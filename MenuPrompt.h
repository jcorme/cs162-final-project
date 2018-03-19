#ifndef ESCAPEFROMCS162_MENUPROMPT_H
#define ESCAPEFROMCS162_MENUPROMPT_H
/*********************************************************************
** Program Filename: MenuPrompt.h
** Author: Jason Chen
** Date: 03/05/2018
** Description: Declares the MenuPrompt template class and its related
 * members.
** Input: None
** Output: None
*********************************************************************/


#include <algorithm>
#include <type_traits>
#include <initializer_list>
#include <vector>
#include <sstream>
#include "PlayerAction.h"
#include "Option.h"
#include "Utils.h"

/*********************************************************************
** Function: CancellationValueForType
** Description: Function template declaration for a function that returns
 * the I value that should map to "cancel."
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename I>
I CancellationValueForType();

template <>
inline char CancellationValueForType<char>() { return '0'; }

template <>
inline unsigned CancellationValueForType<unsigned>() { return 0; }

/*********************************************************************
** Function: CaseInsensitiveCompare
** Description: Function template declaration for a function that compares
 * two input values of type I without regard for case, whatever that may mean
 * in the context of I.
** Parameters: i1 and i2 are the values being compared.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename I>
bool CaseInsensitiveCompare(const I& i1, const I& i2) {
  return i1 == i2;
};

template <>
inline bool CaseInsensitiveCompare<char>(const char& i1, const char& c2) {
  return tolower(i1) == tolower(c2);
}

// MenuPrompt is a (kind of) generic interface for getting input from the user.
// I is the type of input expected from the user; while T are the values that
// the user can select. I must implement the insertion and extraction operators
// in such a way that the user's input can be converted back into the
// corresponding I value. In addition, I must implement the equality comparison.
// and have an explicit specialization of the CancellationValueForType template.
// I can also explicitly specialize the CaseInsensitiveCompare template to
// override its default behavior.
// There are two restrictions on T:
//    1. T must implement the comparison operators.
//    2. T must have an ActionInput specialization, with a corresponding Inputs
//       member map.
// Furthers improvements could include making MenuPrompt generic over some type
// which maps I to T, rather than forcing the use of ActionInput, but that's too
// much work.
template <typename I, typename T>
class MenuPrompt {
  public:
    using ValidationFn = std::function<bool(T)>;

    MenuPrompt() = default;
    // enable_cancel is whether there should be an extra option that allows
    // the user to exit the current menu.
    explicit MenuPrompt(bool enable_cancel): enable_cancel_(enable_cancel) {}
    MenuPrompt(std::initializer_list<T> options);

    void AddOption(T option) { options_.push_back(option); }
    void AddOptions(std::vector<T> options);
    void OverrideInputs(const ActionInputMap<I, T>& overrides);
    void RemoveOption(T option);
    void SetValidationFn(ValidationFn fn) { custom_validation_fn_ = fn; }

    Option<T> ValueForInput(const I& input) const;
    bool InputInRange(const I& input) const;

    void set_print_indented(bool print_indented) {
      print_indented_ = print_indented;
    }

    Option<T> operator()(
        Option<std::string> initial_msg = None,
        Option<std::string> prompt_msg = None,
        Option<std::string> fail_msg = None);

  private:
    std::vector<T> options_;
    // Overrides the default printed option text.
    ActionInputMap<I, T> override_map_;
    bool enable_cancel_ = false;
    bool print_indented_ = false;

    // Overrides the default function that validates user input.
    Option<ValidationFn> custom_validation_fn_;

    void SortOptions();
    void EraseDuplicateOptions();

    const KeyActionPair<I>& DefaultInputFor(T option) const;
    std::string OptionsAsString() const;
    const KeyActionPair<I>& InputFor(T option) const;
};

/*********************************************************************
** Function: MenuPrompt
** Description: Constructor for the MenuPrompt template class.
** Parameters: options is a list of options to give to the user.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename I, typename T>
MenuPrompt<I, T>::MenuPrompt(std::initializer_list<T> options) {
  for (const auto &o : options)
    options_.push_back(o);
}

/*********************************************************************
** Function: AddOptions
** Description: Adds the options from the vector to print to the user.
** Parameters: options to add.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename I, typename T>
void MenuPrompt<I, T>::AddOptions(std::vector<T> options) {
  for (const auto &o : options)
    AddOption(o);
}

/*********************************************************************
** Function: OverrideStrings
** Description: Overrides some, or all, of the default printed option text.
** Parameters: overrides is the map of overrides.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename I, typename T>
void MenuPrompt<I, T>::OverrideInputs(const ActionInputMap<I, T> &overrides){
  override_map_ = overrides;
}

/*********************************************************************
** Function: RemoveOption
** Description: Remove the provided option from the options printed to the user.
** Parameters: option is the option to remove.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename I, typename T>
void MenuPrompt<I, T>::RemoveOption(T option) {
  options_.erase(
      std::remove(options_.begin(), options_.end(), option), options_.end());
}

template <typename I, typename T>
Option<T> MenuPrompt<I, T>::ValueForInput(const I &input) const {
  for (const auto& opt : ActionInput<I, T>::Inputs) {
    if (CaseInsensitiveCompare<I>(opt.second.first, input))
      return opt.first;
  }

  return None;
}

template <typename I, typename T>
bool MenuPrompt<I, T>::InputInRange(const I& input) const {
  for (const auto& opt : ActionInput<I, T>::Inputs) {
    if (std::find(options_.begin(), options_.end(), opt.first)
        != options_.end()) {
      if (CaseInsensitiveCompare<I>(opt.second.first, input))
        return true;
    }
  }

  return false;
}

/*********************************************************************
** Function: SortOptions
** Description: Sorts the provided options from least to greatest, however
 * that may be defined by the type.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename I, typename T>
void MenuPrompt<I, T>::SortOptions()
{
  std::sort(options_.begin(), options_.end(), [](const T &t1, const T &t2) {
      return t1 < t2;
  });
}

/*********************************************************************
** Function: EraseDuplicateOptions
** Description: Removes duplicate options.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename I, typename T>
void MenuPrompt<I, T>::EraseDuplicateOptions() {
  auto fwd_it = std::unique(options_.begin(), options_.end());
  options_.erase(fwd_it, options_.end());
}

/*********************************************************************
** Function: DefaultStringFor
** Description: Retrieves the default option text for the given option.
** Parameters: option is the option to retrieve the text for.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename I, typename T>
const KeyActionPair<I>& MenuPrompt<I, T>::DefaultInputFor(T option) const {
  return ActionInput<I, T>::Inputs.at(option);
}

/*********************************************************************
** Function: OptionsAsString
** Description: Converts the given options into a string presentable to
 * the user.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename I, typename T>
std::string MenuPrompt<I, T>::OptionsAsString() const {
  std::ostringstream oss;

  if (enable_cancel_) {
    if (print_indented_)
      oss << '\t';
    I c_val = CancellationValueForType<I>();
    oss << c_val << ") Cancel" << '\n';
  }

  for (decltype(options_.size()) i = 0; i != options_.size(); ++i) {
    if (print_indented_)
      oss << '\t';
    const KeyActionPair<I>& input = InputFor(options_[i]);
    oss << input.first << ") " << input.second << '\n';
  }

  return oss.str();
}

/*********************************************************************
** Function: StringFor
** Description: Provides the option text for the given option, checking
 * first if it has been overriden and returning the default text if not.
** Parameters: optoin is the option to retrieve the text for.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename I, typename T>
const KeyActionPair<I>& MenuPrompt<I, T>::InputFor(T option) const {
  if (override_map_.empty()) return DefaultInputFor(option);

  if (override_map_.find(option) != override_map_.cend()) {
    return override_map_.at(option);
  }

  return DefaultInputFor(option);
}

/*********************************************************************
** Function: operator()
** Description: Overloads the function call operator; this prompts the
 * user with the given options and returns their choice.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename I, typename T>
Option<T> MenuPrompt<I, T>::operator()(
    Option<std::string> initial_msg,
    Option<std::string> prompt_msg,
    Option<std::string> fail_msg) {
  SortOptions();
  EraseDuplicateOptions();

  if (options_.empty()) return None;

  if (initial_msg.IsSome())
    std::cout << '\n' << initial_msg.CUnwrapRef() << '\n';
  else
    std::cout << "\nChoose an option from below:\n";

  if (!print_indented_)
    std::cout << '\n';

  std::string options_str = OptionsAsString();
  std::cout << options_str << std::endl;

  const std::string& msg = prompt_msg.IsSome() ? prompt_msg.CUnwrapRef()
                                               : "Enter option: ";

  I c_val = CancellationValueForType<I>();
  I choice = PromptUntilValid<I>(
      msg,
      [&](const I& input) {
          if (enable_cancel_ && CaseInsensitiveCompare<I>(c_val, input)) {
            return true;
          }

          bool in_range = InputInRange(input);
          if (custom_validation_fn_.IsSome()) {
            return in_range &&
                custom_validation_fn_.CUnwrapRef()(
                    ValueForInput(input).Unwrap());
          }
          return in_range;
      },
      fail_msg
  );

  if (choice == c_val) return None;
  return ValueForInput(choice).Unwrap();
}


#endif //ESCAPEFROMCS162_MENUPROMPT_H
