#ifndef VALIDATORTRAITS_H
#define VALIDATORTRAITS_H

#include <QtCore>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QButtonGroup>
#include "ValidationResult.h"


/// @brief default generic version that is invalid
template <typename T>
class ValidatorTraits {
    static constexpr bool has_default = false;
};


// --- QLineEdit Specialization ---

/// @brief specialization for QLineEdit, default signal is textChanged
template <>
class ValidatorTraits<QLineEdit> {
    static constexpr bool has_default = true;

    /// @brief default signal used is textChanged
    static constexpr auto default_signal() {
        return &QLineEdit::textChanged;
    };

/**
 * @brief Validates that a QLineEdit contains non-whitespace text.
 *
 * Performs a trim-and-length check on the provided widget's text.
 * This is a static utility function used for basic "required field" validation.
 *
 * @param[in] wdg  A pointer to the QLineEdit to be validated.
 *                 Must not be @c nullptr.
 *
 * @return A @c ValidationResult indicating success if the trimmed length
 *         is > 0, or a @c ValidationResult::Failure with an error message
 *         if the field is empty or contains only whitespace.
 *
 * @note This function uses @c QString::trimmed() to ensure that strings
 *       consisting only of spaces are treated as empty.
 */
    static ValidationResult default_check(QLineEdit* wdg) {
        if ( !wdg ) {
            return ValidationResult::Failure("Internal Error - empty object");
        }

        if ( wdg->text().trimmed().length() <= 0 ){
            return ValidationResult::Failure("The text cannot be empty");
        }

        return ValidationResult::Success();
    }
};


// --- QComboBox Specialization ---

/// @brief specialization for QComboBox, default signal is currentIndexChanged
template <>
class ValidatorTraits<QComboBox> {
    static constexpr bool has_default = true;

    /**
    * @brief Provides the address of the standard selection-change signal for a QComboBox.
    *
    * This constant expression resolves the overloaded @c currentIndexChanged signal
    * to its integer-based signature. It serves as the primary hook for validation
    * logic triggered by user selection.
    *
    * @return A pointer to the member function @c QComboBox::currentIndexChanged(int).
    *
    * @note This uses @c QOverload<int>::of() to ensure the compiler selects the
    *       integer overload, maintaining compatibility between Qt 5.15 and Qt 6.x.
    */
    static constexpr auto default_signal() {
        return QOverload<int>::of(&QComboBox::currentIndexChanged);
    };

    /**
    * @brief Validates that a QComboBox has a valid selection.
    *
    * Checks if the user has selected an item from the drop-down menu.
    * This is a static utility used to ensure a choice has been made in a form.
    *
    * @param[in] wdg  A pointer to the QComboBox to be validated.
    *                 Must not be @c nullptr.
    *
    * @return A @c ValidationResult indicating success if an item is selected
    *         (index >= 0), or a @c ValidationResult::Failure if no item is
    *         selected or the pointer is @c nullptr.
    *
    * @note In Qt, an index of -1 typically represents "no selection" or an
    *       empty editable combo box.
    */
    static ValidationResult default_check(QComboBox* wdg) {
        if ( !wdg ) {
            return ValidationResult::Failure("Internal Error - empty object");
        }

        if ( wdg->currentIndex() < 0 ){
            return ValidationResult::Failure("An option must be selected from the drop-down menu");
        }

        return ValidationResult::Success();
    }
};


// --- QCheckBox Specialization ---

/// @brief specialization for QCheckBox, default signal is toggled
template <>
class ValidatorTraits<QCheckBox> {
    static constexpr bool has_default = true;

    /**
    * @brief Provides the address of the standard state-change signal for a QCheckBox.
    *
    * This constant expression returns the address of the @c toggled signal.
    * It serves as the primary hook for validation logic triggered when the
    * checkbox state is flipped by the user.
    *
    * @return A pointer to the member function @c QCheckBox::toggled(bool).
    *
    * @note Unlike @c stateChanged(int), @c toggled(bool) is the preferred
    *       signal for simple Boolean checks.
    */
    static constexpr auto default_signal() {
        return &QCheckBox::toggled;
    };

    /**
    * @brief Validates that a QCheckBox is in the checked state.
    *
    * Checks if the user has ticked the checkbox. This is a static utility
    * used to ensure mandatory agreements or confirmations are accepted.
    *
    * @param[in] wdg  A pointer to the QCheckBox to be validated.
    *                 Must not be @c nullptr.
    *
    * @return A @c ValidationResult indicating success if the checkbox is
    *         checked, or a @c ValidationResult::Failure if it is unchecked
    *         or the pointer is @c nullptr.
    *
    * @note This is commonly used for "Terms of Service" or "I Agree" fields.
    */
    static ValidationResult default_check(QCheckBox* wdg) {
        if ( !wdg ) {
            return ValidationResult::Failure("Internal Error - empty object");
        }

        if ( !wdg->isChecked() ) {
            return ValidationResult::Failure("You must accept this field to proceed");
        }

        return ValidationResult::Success();
    }
};


// --- QButtonGroup Specialization ---

/// @brief specialization for QButtonGroup, default signal is idClicked
template <>
class ValidatorTraits<QButtonGroup> {
    static constexpr bool has_default = true;

    /**
    * @brief Provides the address of the standard selection signal for a QButtonGroup.
    *
    * This constant expression resolves the overloaded @c idClicked signal
    * to its integer-based signature. It serves as the primary hook for
    * validation logic triggered when a button within the group is selected.
    *
    * @return A pointer to the member function @c QButtonGroup::idClicked(int).
    *
    * @note This uses @c QOverload<int>::of() to ensure compatibility with
    *       Qt 5.15 and Qt 6.x, focusing on the ID of the clicked button.
    */
    static constexpr auto default_signal() {
        return QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked);
    };

    /**
    * @brief Validates that a QButtonGroup has a valid selection.
    *
    * Checks if at least one button within the logical group is currently
    * selected. This is used to ensure a choice is made in radio button groups.
    *
    * @param[in] wdg  A pointer to the QButtonGroup to be validated.
    *                 Must not be @c nullptr.
    *
    * @return A @c ValidationResult indicating success if a button is checked
    *         (checkedId != -1), or a @c ValidationResult::Failure if no
    *         selection is made or the pointer is @c nullptr.
    *
    * @note In Qt, @c checkedId() returns -1 if no button in the group is
    *       actively selected.
    */
    static ValidationResult default_check(QButtonGroup* wdg) {
        if ( !wdg ) {
            return ValidationResult::Failure("Internal Error - empty object");
        }

        if ( wdg->checkedId() == -1 ) {
            return ValidationResult::Failure("Please select one of the available options");
        }

        return ValidationResult::Success();
    }
};






#endif // VALIDATORTRAITS_H
