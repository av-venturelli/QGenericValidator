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


signals:
    void validationFailed(QWidget* wdg, QString const& message);
    void formValidityChanged(bool isValid);
public slots:
    void process();

/*====================================
 *        TYPE ERASURE PATTERN
======================================*/
private:
    // abstract base class
    struct IFieldValidator {
        virtual ~IFieldValidator();

        virtual ValidationResult check() = 0;
        virtual QWidget* get_widget() const = 0;
        virtual void disconnect_signal() = 0;
    };

    // model class implementation
    template <typename WdgT>
    struct FieldModel : public IFieldValidator {
        using ValidatorFunc = std::function<ValidationResult(WdgT)>;

        // ctor
        FieldModel(
            WdgT* widget,
            ValidatorFunc validator_func,
            QMetaObject::Connection connection
        );

        virtual ~FieldModel();
        virtual ValidationResult check() override;
        virtual QWidget* get_widget() const override;
        virtual void disconnect_signal() override;

        // members
        QPointer<WdgT> wdg_ptr;
        ValidatorFunc validator_f;
        QMetaObject::Connection connection;
    };

/*====================================
*            PRIVATE MEMBERS
======================================*/
private:
    std::vector<std::unique_ptr<IFieldValidator>> m_vect_validator;
};

#endif // VALIDATIONMANAGER_H
