#ifndef VALIDATIONMANAGER_H
#define VALIDATIONMANAGER_H

#include <QObject>
#include <QWidget>
#include <QPointer>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <QDebug>

#include "ValidationResult.h"
#include "ValidatorTraits.h"

class ValidationManager : public QObject
{
    Q_OBJECT

public:
    explicit ValidationManager(QObject *parent = nullptr) : QObject{parent} {}
    ~ValidationManager() = default;

    bool isValid() const { return m_isValid; }

    // 3-Argument Registration
    template <typename QObjT, typename Signal, typename ValidatorFunc>
    bool registerQObject(QObjT *q_obj, Signal q_signal_trigger, ValidatorFunc validator_func) {
        static_assert(std::is_base_of_v<QObject, QObjT>, "Wdg must derive from QObject.");
        static_assert(std::is_member_function_pointer<Signal>::value, "Signal must be a pointer to a member function.");

        if (!q_obj) return false;

        // Check for duplicates
        auto it = std::find_if(m_vect_validator.begin(), m_vect_validator.end(),
                               [q_obj](const std::unique_ptr<IFieldValidator>& ptr) {
                                   return ptr->get_qobject() == q_obj;
                               });

        if (it != m_vect_validator.end()) return false;

        // Connect to the central processor
        QMetaObject::Connection conn = QObject::connect(
            q_obj, q_signal_trigger, this, &ValidationManager::process);

        if (!conn) return false;

        m_vect_validator.emplace_back(
            std::make_unique<FieldModel<QObjT>>(q_obj, validator_func, conn)
            );
        return true;
    }

    // 1-Argument Registration (Uses Traits)
    template <typename QObjT>
    bool registerQObject(QObjT* q_obj) {
        static_assert(ValidatorTraits<QObjT>::has_default,
                      "No default ValidatorTraits defined for this type.");

        return registerQObject(
            q_obj,
            ValidatorTraits<QObjT>::default_signal(),
            &ValidatorTraits<QObjT>::default_check
            );
    }

signals:
    void validationFailed(QObject* qobj, QString const& message);
    void formValidityChanged(bool isValid);
    void validationsSuccessed();

public slots:
    void process() {
        removeZombieObjects();

        if (m_vect_validator.empty()) {
            if (!m_isValid) setIsValid(true);
            return;
        }

        ValidationResult res = ValidationResult::Success();
        for (auto& ifield_validator : m_vect_validator) {
            res = ifield_validator->check();
            if (!res.isValid) {
                emit validationFailed(ifield_validator->get_qobject(), res.errorMessage);
                break;
            }
        }

        setIsValid(res.isValid);
        if (isValid()) emit validationsSuccessed();
    }

private:
    /*====================================
    * TYPE ERASURE PATTERN
    ======================================*/
    struct IFieldValidator {
        virtual ~IFieldValidator() = default;
        virtual ValidationResult check() = 0;
        virtual QObject* get_qobject() const = 0;
        virtual void disconnect_signal() = 0;
    };

    template <typename QObjT>
    struct FieldModel : public IFieldValidator {
        using ValidatorFunc = std::function<ValidationResult(QObjT*)>;

        QPointer<QObjT> qobj_ptr;
        ValidatorFunc validator_f;
        QMetaObject::Connection connection;

        FieldModel(QObjT* q_obj, ValidatorFunc validator_func, QMetaObject::Connection connection)
            : qobj_ptr(q_obj), validator_f(std::move(validator_func)), connection(connection) {}

        ~FieldModel() override = default;

        ValidationResult check() override {
            if (qobj_ptr.isNull()) {
                return ValidationResult::Failure("Checking a deleted object");
            }
            return validator_f(qobj_ptr.data());
        }

        QObject* get_qobject() const override {
            return qobj_ptr.data();
        }

        void disconnect_signal() override {
            if (connection) QObject::disconnect(connection);
        }
    };

    /*====================================
    * PRIVATE HELPERS
    ======================================*/
    void setIsValid(bool isValid) {
        if (isValid == m_isValid) return;
        m_isValid = isValid;
        emit formValidityChanged(m_isValid);
    }

    void removeZombieObjects() {
        m_vect_validator.erase(
            std::remove_if(m_vect_validator.begin(), m_vect_validator.end(),
                           [](const std::unique_ptr<IFieldValidator>& v) {
                               return v->get_qobject() == nullptr;
                           }),
            m_vect_validator.end()
            );
    }

    std::vector<std::unique_ptr<IFieldValidator>> m_vect_validator;
    bool m_isValid{false};
};

#endif // VALIDATIONMANAGER_H