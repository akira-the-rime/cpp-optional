#include <stdexcept>
#include <utility>

class BadOptionalAccess final : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() noexcept = default;

    Optional(const T& value) {
        value_ = new (&data_[0]) T(value);
        is_initialized_ = true;
    }

    Optional(T&& value) noexcept {
        value_ = new (&data_[0]) T(std::move(value));
        is_initialized_ = true;
    }

    Optional(const Optional& other) {
        if (other.HasValue()) {
            this->value_ = new (&data_[0]) T(*other.value_);
            is_initialized_ = true;
        }
    }

    Optional(Optional&& other) noexcept {
        if (other.HasValue()) {
            this->value_ = new (&data_[0]) T(std::move(*other.value_));
            is_initialized_ = true;
        }
    }

    Optional& operator=(const T& value) {
        if (is_initialized_) {
            *value_ = value;
        }
        else {
            value_ = new (&data_[0]) T(value);
            is_initialized_ = true;
        }

        return *this;
    }

    Optional& operator=(T&& value) noexcept {
        if (is_initialized_) {
            *value_ = std::move(value);
        }
        else {
            this->value_ = new (&data_[0]) T(std::move(value));
            is_initialized_ = true;
        }

        return *this;
    }

    Optional& operator=(const Optional& other) {
        if (other.HasValue()) {
            if (this->is_initialized_) {
                *this->value_ = *other.value_;
            }
            else {
                this->value_ = new (&data_[0]) T(*other.value_);
                is_initialized_ = true;
            }
        }
        else {
            this->Reset();
        }

        return *this;
    }

    Optional& operator=(Optional&& other) noexcept {
        if (other.HasValue()) {
            if (this->is_initialized_) {
                *this->value_ = std::move(*other.value_);;
            }
            else {
                this->value_ = new (&data_[0]) T(std::move(*other.value_));
                is_initialized_ = true;
            }
        }
        else {
            this->Reset();
        }

        return *this;
    }

    ~Optional() noexcept {
        if (is_initialized_) {
            value_->~T();
        }
    }

    template <class... Args>
    void Emplace(Args&&... args) {
        Reset();

        value_ = new (&data_[0]) T(std::forward<Args>(args)...);
        is_initialized_ = true;
    }

    bool HasValue() const noexcept {
        return is_initialized_;
    }

    T& operator*()& {
        return *value_;
    }

    const T& operator*() const& {
        return *value_;
    }

    T operator*()&& {
        return std::move(*value_);
    }

    const T& operator*() const&& {
        return std::move(*value_);
    }

    T* operator->() {
        return value_;
    }

    const T* operator->() const {
        return value_;
    }

    T& Value()& {
        if (!is_initialized_) {
            throw BadOptionalAccess();
        }

        return *value_;
    }

    const T& Value() const& {
        if (!is_initialized_) {
            throw BadOptionalAccess();
        }

        return *value_;
    }

    T Value()&& {
        if (!is_initialized_) {
            throw BadOptionalAccess();
        }

        return std::move(*value_);
    }

    T Value() const&& {
        if (!is_initialized_) {
            throw BadOptionalAccess();
        }

        return std::move(*value_);
    }

    void Reset() noexcept {
        if (is_initialized_) {
            value_->~T();
            value_ = nullptr;
            is_initialized_ = false;
        }
    }

private:
    alignas(T) char data_[sizeof(T)];
    T* value_ = nullptr;
    bool is_initialized_ = false;
};