#include "validationManager.h"

ValidationManager::ValidationManager(QObject *parent)
    : QObject{parent}
{}

ValidationManager::~ValidationManager()
{}

void ValidationManager::process()
{

}




/*====================================
 *        TYPE ERASURE PATTERN
======================================*/
ValidationManager::IFieldValidator::~IFieldValidator() = default;

template<typename WdgT>
ValidationManager::FieldModel<WdgT>::FieldModel(
    WdgT *widget,
    ValidatorFunc validator_func,
    QMetaObject::Connection connection
) : wdg_ptr(QPointer<WdgT>(widget)),
    validator_f(std::move(validator_func)),
    connection(connection)
{
}


template<typename WdgT>
ValidationManager::FieldModel<WdgT>::~FieldModel() = default;


template<typename WdgT>
ValidationResult ValidationManager::FieldModel<WdgT>::check()
{
    if ( wdg_ptr->isNull() ){
        return ValidationResult::Failure("Checking a nullptr widget");
    }

    return validator_f( wdg_ptr->data() );
}


template<typename WdgT>
QWidget *ValidationManager::FieldModel<WdgT>::get_widget() const
{
    return wdg_ptr->data();
}


template<typename WdgT>
void ValidationManager::FieldModel<WdgT>::disconnect_signal()
{
    QObject::disconnect( connection );
}
