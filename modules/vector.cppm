module;

#include <array>
#include <cstddef>
#include <meta>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

export module cosinet.vector;

export namespace cosinet
{

// Generates dimension named reference members.
template <class type, std::size_t dimension>
struct vector_members
{
    static_assert(dimension <= 26);

    struct named;

    consteval
    {
        std::vector<std::meta::info> members;

        for(std::size_t i = 0; i < dimension; ++i)
        {
            std::string name(1, char('a' + i));

            members.push_back(
                std::meta::data_member_spec(
                    ^^std::add_lvalue_reference_t<type>,
                    {
                        .name = name
                    }
                )
            );
        }

        std::meta::define_aggregate(^^named, members);
    }
};

// Generic vector for every dimension.
template <class type, std::size_t dimension>
struct vector : private std::array<type, dimension>, public vector_members<type, dimension>::named
{
private:
    using storage = std::array<type, dimension>;
    using names = vector_members<type, dimension>::named;

    template <std::size_t... I>
    constexpr vector(const storage& values, std::index_sequence<I...>)
        : storage(values),
          names{storage::operator[](I)...}
    {
    }

public:
    constexpr vector()
        : vector(storage{}, std::make_index_sequence<dimension>{})
    {
    }

    constexpr vector(const vector& other)
        : vector(other.to_array(), std::make_index_sequence<dimension>{})
    {
    }

    template <class... Args>
        requires(sizeof...(Args) == dimension && (std::convertible_to<Args, type> && ...))
    constexpr vector(Args&&... args)
        : vector(storage{static_cast<type>(std::forward<Args>(args))...}, std::make_index_sequence<dimension>{})
    {
    }

    constexpr vector& operator=(const vector& other)
    {
        to_array() = other.to_array();
        return *this;
    }

    constexpr storage& to_array()
    {
        return static_cast<storage&>(*this);
    }

    constexpr const storage& to_array() const
    {
        return static_cast<const storage&>(*this);
    }

    constexpr type* data()
    {
        return to_array().data();
    }

    constexpr const type* data() const
    {
        return to_array().data();
    }

    constexpr type& operator[](std::size_t i)
    {
        return to_array()[i];
    }

    constexpr const type& operator[](std::size_t i) const
    {
        return to_array()[i];
    }
};

} // namespace cosinet