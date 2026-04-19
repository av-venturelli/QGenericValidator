#ifndef VALIDATIONMANAGER_H
#define VALIDATIONMANAGER_H

#include <QObject>
#include <QWidget>
#include "ValidationResult.h"

class ValidationManager : public QObject
{
    Q_OBJECT

public:
    explicit ValidationManager(QObject *parent = nullptr);
    ~ValidationManager();

    /*!
     * @brief returns the current validation state
    */
    bool isValid() const;

    /*!
     * @brief Register a QObject based type and when the specidied signal il triggered, validate
     * all the registered qobjects executing the @p validator_func provided
     * @param[in] q_obj -> a pointer to a QObject or a subclass of it.
     * @param[in] q_signal_tirgger -> the signal that will trigger the validation process.
     * @param[in] validator_func -> the callback used to determine if the passed widget is valid or not.
    */
    template <typename QObj, typename Signal>
    bool registerQObject( QObj* q_obj, Signal q_signal_trigger, std::function<ValidationResult(QObj*)> validator_func);

    template <typename QObj>
    bool registerQObject( QObj* q_obj);

signals:
    void validationFailed(QObject* qobj, QString const& message);
    void formValidityChanged(bool isValid);
    void validationsSuccessed();
public slots:
    /*!
     * @brief checks all the registered widgets' validity.
     * @b validationFailed(QWidget*, QString const&) is emitted at the exact moment when
     * the validation returns false and exit the function.
     * @b formValidityChanged(bool isValid) signal is emitted wether the state chenged
     * from true to false or the other way around.
    */
    void process();

/*====================================
*        TYPE ERASURE PATTERN
======================================*/
private:
    // abstract base class
    struct IFieldValidator {
        virtual ~IFieldValidator();

        virtual ValidationResult check() = 0;
        virtual QObject* get_qobject() const = 0;
        virtual void disconnect_signal() = 0;
    };

    // model class implementation
    template <typename QObjT>
    struct FieldModel : public IFieldValidator {
        using ValidatorFunc = std::function<ValidationResult(QObjT)>;

        // ctor
        FieldModel(
            QObjT* q_obj,
            ValidatorFunc validator_func,
            QMetaObject::Connection connection
        );

        virtual ~FieldModel();
        virtual ValidationResult check() override;
        virtual QObject* get_qobject() const override;
        virtual void disconnect_signal() override;

        // members
        QPointer<QObjT> qobj_ptr;
        ValidatorFunc validator_f;
        QMetaObject::Connection connection;
    };

/*====================================
*            PRIVATE MEMBERS
======================================*/
private:
    std::vector<std::unique_ptr<IFieldValidator>> m_vect_validator;
    bool m_isValid{};


    // private members' func
    //-----------------------
    void setIsValid(bool isValid);

    /*!
     * @brief When an QObject is detroyed by the user for some reason, the QPOinter<ToThatQObject>
     * will became invalid (nullptr) so there is no need to keep it inside the collection.
    */
    void removeZombieObjects();
};

#endif // VALIDATIONMANAGER_H
