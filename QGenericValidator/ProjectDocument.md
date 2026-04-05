# Project Specification: Type-Erased Qt Validation Engine

## 1. Executive Summary
**Goal:** Create a non-intrusive, header-only (or lightweight library) validation engine for Qt applications.
**Core Tech:** C++17, Qt 5.15/6.x, Type Erasure (Concept/Model), Template Meta-programming (Traits).
**Mission:** Allow developers to register any QWidget for validation with zero boilerplate, full type safety at compile-time, and no modification to existing widget classes.

---

## 2. Strategic Architecture

### 2.1 The "Concept/Model" Type Erasure
To store different types (e.g., `QLineEdit`, `QCheckBox`) in a single `std::vector`, we use a type-erasure wrapper. 

| Component | Role |
| :--- | :--- |
| **IFieldValidator** (Concept) | An abstract base class (interface) with a virtual `check()` method. |
| **FieldModel<W, F>** (Model) | A templated implementation that stores a specific `Widget*` and a specific `Functor`. |

### 2.2 The Traits Dictionary
Instead of hardcoding logic inside the manager, we use `ValidatorTraits<T>`.
*   **Static Mapping:** Maps `Type` -> `Default Signal` & `Default Logic`.
*   **Compile-Time Safety:** Uses `static_assert` to prevent unsupported widgets from being added without custom logic.

---

## 3. Component Deep-Dive

### 3.1 ValidationResult
A simple data-carrying struct to pass state from the logic to the UI.
*   `bool isValid`
*   `QString errorMessage`

### 3.2 ValidatorTraits<T> (The Strategy)
**Reasoning:** Using a trait-based dictionary ensures that the "Default" behavior for a `QLineEdit` is resolved by the compiler. This avoids `if/else` chains and `qobject_cast` at runtime.

### 3.3 ValidationManager (The Controller)
**Reasoning:** The manager acts as the "Brain." It holds the collection of erased validators and coordinates the "Short-Circuit" evaluation (stopping at the first error to improve UX).

---

## 4. Development Roadmap & Priorities

| Priority | Phase | Tasks |
| :--- | :--- | :--- |
| **1** | **Foundation** | Define `ValidationResult` and `ValidatorTraits`. Implement specialization for `QLineEdit`. |
| **2** | **Erasure** | Build the `IFieldValidator` interface and the templated `FieldModel` wrapper. |
| **3** | **Logic** | Implement `ValidationManager::add()` overloads and the `process()` loop. |
| **4** | **Safety** | Integrate `QPointer<T>` to handle dynamic widget destruction and signal disconnection. |
| **5** | **Edge Cases** | Handle overloaded Qt signals (PMF resolution) and unit testing. |

---

## 5. Expected Usage (Pseudo-Code)
```cpp
// Initialize
auto* vm = new ValidationManager(this);

// Default validation (Traits-based)
vm->add(ui->emailLineEdit); 

// Custom validation (Lambda-based)
vm->add(ui->passwordLineEdit, [](QLineEdit* w) {
    return w->text().length() >= 8 ? Success() : Failure("Too short!");
});

// Reactivity
connect(vm, &ValidationManager::formValidityChanged, ui->submitButton, &QPushButton::setEnabled);