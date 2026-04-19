#include "validationManager.h"
#include "ValidatorTraits.h"

ValidationManager::ValidationManager(QObject *parent)
    : QObject{parent}
{}

ValidationManager::~ValidationManager()
{}

bool ValidationManager::isValid() const
{
    return m_isValid;
}

void ValidationManager::setIsValid(bool isValid)
{
    // already setted
    if ( isValid == this->isValid() ) {
        return;
    }

    m_isValid = isValid;
    emit formValidityChanged( m_isValid );
}

void ValidationManager::removeZombieObjects()
{
    // std::remove_if moves the 'dead' elements to the end of the vector
    // the QObject distructor already disconnects the signals
    m_vect_validator.erase(
        std::remove_if(
            m_vect_validator.begin(), m_vect_validator.end(),
               [](const std::unique_ptr<IFieldValidator>& v) {
                   // If get_qobject() returns nullptr, the QPointer inside is dead.
                   return v->get_qobject() == nullptr;
                }),
        m_vect_validator.end()
    );
}

void ValidationManager::process() {
    removeZombieObjects();

    // like std::all_of if the collection
    // is empty returns true
    if ( m_vect_validator.size() == 0 ){
        qWarning() << "[Warning] ValidationManger does not have any Widget registered";
        if( !m_isValid) {
            setIsValid(true);
        }
    }

    ValidationResult _res{};
    for ( auto& ifield_validator : m_vect_validator) {
        _res = ifield_validator->check();
        if ( !_res.isValid ) {
            emit validationFailed( ifield_validator->get_qobject(), _res.errorMessage );
            break;
        }
    }
    // update member
    this->setIsValid( _res.isValid );
    if ( isValid() ) emit validationsSuccessed();
}


template<typename QObj, typename Signal>
bool ValidationManager::registerQObject(QObj *q_obj, Signal q_signal_trigger, std::function<ValidationResult(QObj *)> validator_func)
{
    // compile time assertions
    static_assert( std::is_base_of_v<QObject, QObj>, "Wdg must derived from QObject at least.");
    static_assert(std::is_member_function_pointer<Signal>::value,
                  "ValidationManager Error: Signal must be a pointer to a member function (e.g., &QLineEdit::textChanged).");

    if (!q_obj) {
        qWarning() << "[ValidationManager::registerWidget] - Trying to register a nullptr QObject";
        return false;
    }

    auto it = std::find_if(
        std::begin(m_vect_validator),
        std::end(m_vect_validator),
        [q_obj](std::unique_ptr<IFieldValidator> const& ptr) -> bool { return ptr.get() == q_obj; }
    );
    if( it != std::end(m_vect_validator) ) {
        // already registered
        return false;
    }

    // The "Implicit Check" via Qt PMF Connect
    // This is where we verify if the signal is valid.
    // If 'signal' is not a member function of Wdg (or a parent),
    // or if the signatures are fundamentally incompatible, this line will NOT compile.
    QMetaObject::Connection conn = QObject::connect(
        q_obj,
        q_signal_trigger,
        this,
        &ValidationManager::process // Every signal trigger, calls our central processing logic
    );

    // 3. Final Runtime Guard
    if (!conn) {
        qWarning("ValidationManager: Connection failed. Ensure the signal exists and is accessible.");
        return false;
    }

    // 4. Wrap into Type-Erasure container
    m_vect_validator.emplace_back(
        std::make_unique<FieldModel<QObj>>(q_obj, validator_func, conn)
    );
    return true;
}

template <typename QObj>
bool ValidationManager::registerQObject(QObj* q_obj) {
    // 1. The "Static Guard"
    // This checks the trait you defined in ValidatorTraits.h
    static_assert(ValidatorTraits<QObj>::has_default,
                  "ERROR: No default ValidatorTraits defined for this type. "
                  "Either provide the signal and function manually, or specialize ValidatorTraits.");

    // 2. The "Delegation"
    // We call the full 3-argument version. This avoids logic duplication.
    return registerWidget(
        q_obj,
        ValidatorTraits<QObj>::default_signal(),
        &ValidatorTraits<QObj>::default_check
    );
}



/*====================================
 *        TYPE ERASURE PATTERN
======================================*/
ValidationManager::IFieldValidator::~IFieldValidator() = default;

template<typename QObjT>
ValidationManager::FieldModel<QObjT>::FieldModel(
    QObjT *q_obj,
    ValidatorFunc validator_func,
    QMetaObject::Connection connection
    ) : qobj_ptr(QPointer<QObjT>(q_obj)),
    validator_f(std::move(validator_func)),
    connection(connection)
{
}


template<typename QObjT>
ValidationManager::FieldModel<QObjT>::~FieldModel() = default;


template<typename QObjT>
ValidationResult ValidationManager::FieldModel<QObjT>::check()
{
    if ( qobj_ptr->isNull() ){
        return ValidationResult::Failure("Checking a nullptr qobject");
    }

    return validator_f( qobj_ptr->data() );
}


template<typename WdgT>
QObject *ValidationManager::FieldModel<WdgT>::get_qobject() const
{
    return qobj_ptr->data();
}


template<typename WdgT>
void ValidationManager::FieldModel<WdgT>::disconnect_signal()
{
    QObject::disconnect( connection );
}

/*============================================================*/




