#pragma once

enum class Status {
    Success,
    NullArgument,
    BadArgument,
    GenericFailure // Something went wrong, but we don't know what exactly
};

template<typename T>
class Result {
public:
    Result(Status status, std::string message)
        : _status(status), _message(std::move(message)) {
    }

    Result(Status status, std::string message, const T& value)
        : _status(status), _message(std::move(message)), _value(value) {
    }

    Result(Status status) : _status(status), _message("Error message not provided"), _value(std::nullopt) { }

    Result(const T& value) : _status(Status::Success), _value(value) {
    }

    bool isSuccess() const {
        return _status == Status::Success;
    }

    Status status() const {
        return _status;
    }

    std::string message() const {
        return _message;
    }

    T value() const {
        return _value.value();
    }

private:
    Status _status;
    std::string _message;
    std::optional<T> _value;
};