#ifndef VALIDATIONRESULT_H
#define VALIDATIONRESULT_H
#include <QString>

struct ValidationResult {
    bool isValid;
    QString errorMessage;

    /// @brief create a valid result. Convenience method
    static ValidationResult Success() {
        return ValidationResult{ true, QString("") };
    }

    /// @brief create a invalid result. Convenience method
    /// @param[in] msg the message to embed in the result
    static ValidationResult Failure(QString const& msg) {
        return ValidationResult{ false, msg };
    }
};



#endif // VALIDATIONRESULT_H
