/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <concepts>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

export module Helium.Base.HeliumSharedObject;

import Helium.Base.HeliumObject;

namespace helium::base::details
{
template <typename T>
constexpr inline bool is_const_v = false;

template <typename T>
constexpr inline bool is_const_v<const T> = true;
} // namespace helium::base::details

export namespace helium::base
{
template <typename T>
class WeakRef
{
public:
    using RefType = T;

private:
    using StateType = typename RefType::StateType;
    using PointeeType = std::conditional_t<details::is_const_v<RefType>, const StateType, StateType>;
    std::weak_ptr<PointeeType> weak_ptr_;

public:
    WeakRef(RefType const &ref) : weak_ptr_(ref.state_)
    {
    }
    [[nodiscard]] auto lock() const noexcept
    {
        return RefType::_try_lock(this->weak_ptr_);
    }
    [[nodiscard]] auto useCount() const noexcept -> long
    {
        return this->weak_ptr_.use_count();
    }
};

template <typename T>
class RefToConst
{
public:
    using RefType = T;
    using StateType = typename RefType::StateType;

private:
    std::shared_ptr<StateType> state_;

public:
    RefToConst(RefType const &ref) noexcept : state_(ref.state_)
    {
    }

    auto operator->() const noexcept -> const StateType *
    {
        return this->state_.get();
    }

    operator StateType const &() const noexcept
    {
        return *this->state_;
    }

    [[nodiscard]] auto clone() const noexcept -> RefType
    {
        return RefType::_from_ptr(this->state_).clone();
    }

    auto unshare() noexcept -> void
    {
        this->state_ = this->clone().state_;
    }

    [[nodiscard]] auto weakRef() const noexcept -> WeakRef<RefType const>
    {
        return RefType::_from_ptr(this->state_);
    }

    operator WeakRef<RefType const>() const noexcept
    {
        return this->weakRef();
    }

    [[nodiscard]] auto useCount() const noexcept -> long
    {
        return this->state_.use_count();
    }
};

template <typename Derived_, typename SharedState_>
class HeliumSharedObjectBase : public HeliumObject
{
public:
    using StateType = SharedState_;

private:
    struct _emplace_ptr_tag
    {
    };
    using Derived = Derived_;

    friend WeakRef<Derived>;
    friend WeakRef<Derived const>;
    friend RefToConst<Derived>;

    std::shared_ptr<StateType> state_;

    auto _as_derived() noexcept -> auto &
    {
        return static_cast<Derived &>(*this);
    }
    auto _as_derived() const noexcept -> auto &
    {
        return static_cast<Derived const &>(*this);
    }

    static auto _from_ptr(std::shared_ptr<StateType> ptr) noexcept -> Derived
    {
        static_assert(std::constructible_from<Derived, _emplace_ptr_tag, std::shared_ptr<StateType>>,
                      "The HeliumSharedObjectBase<> derived class must explicitly inherit the constructors of the "
                      "HeliumSharedObjectBase<> base class.");
        return Derived{_emplace_ptr_tag{}, std::move(ptr)};
    }

    static constexpr auto _try_lock(std::weak_ptr<StateType> const &weak) noexcept -> std::optional<Derived>
    {
        std::optional<Derived> result;
        if (auto ptr = weak.lock())
        {
            result.emplace(HeliumSharedObjectBase::_from_ptr(std::move(ptr)));
        }
        return result;
    }

    static constexpr auto _try_lock(std::weak_ptr<StateType const> const &weak) noexcept -> std::optional<Derived>
    {
        std::optional<Derived> result;
        if (auto ptr = weak.lock())
        {
            result.emplace(HeliumSharedObjectBase::_from_ptr(std::const_pointer_cast<StateType>(std::move(ptr))));
        }
        return result;
    }

public:
    HeliumSharedObjectBase() noexcept(noexcept(StateType{})) : state_(std::make_shared<StateType>())
    {
    }

    HeliumSharedObjectBase(_emplace_ptr_tag, std::shared_ptr<StateType> &&ptr) noexcept : state_(std::move(ptr))
    {
    }

    [[deprecated("Conversioon would remove 'const' qualifier on reference")]]
    HeliumSharedObjectBase(RefToConst<Derived>) = delete;

    auto operator->() const noexcept -> StateType *
    {
        return this->state_.get();
    }

    operator HeliumSharedObjectBase &() const noexcept
    {
        return *this->state_;
    }

    [[nodiscard]] static auto cloneFrom(StateType const &state) noexcept(noexcept(StateType{state})) -> Derived
    {
        return HeliumSharedObjectBase::_from_ptr(std::make_shared<StateType>(state));
    }

    [[nodiscard]] static auto cloneFrom(StateType &&state) noexcept(noexcept(StateType{std::move(state)})) -> Derived
    {
        return HeliumSharedObjectBase::_from_ptr(std::make_shared<StateType>(std::move(state)));
    }

    [[nodiscard]] auto clone() const noexcept -> Derived
    {
        return HeliumSharedObjectBase::cloneFrom(*this->state_);
    }

    auto unshare() noexcept -> void
    {
        this->_as_derived() = this->clone();
    }

    [[nodiscard]] auto useCount() const noexcept -> long
    {
        return this->state_.use_count();
    }
};

template <typename Derived_, typename SharedState_>
WeakRef(HeliumSharedObjectBase<Derived_, SharedState_> const &) -> WeakRef<Derived_>;

template <typename Derived_, typename SharedState_>
RefToConst(HeliumSharedObjectBase<Derived_, SharedState_> const &) -> RefToConst<Derived_>;
} // namespace helium::base
