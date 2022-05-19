
#ifndef REXSAPI_VALUE_DETAILS_HXX
#define REXSAPI_VALUE_DETAILS_HXX

#include <rexsapi/Types.hxx>

#include <variant>

namespace rexsapi
{
  template<class... Ts>
  struct overload : Ts... {
    using Ts::operator()...;
  };
  template<class... Ts>
  overload(Ts...) -> overload<Ts...>;

  namespace detail
  {
    using Variant =
      std::variant<std::monostate, double, Bool, int64_t, std::string, std::vector<double>, std::vector<Bool>,
                   std::vector<int64_t>, std::vector<std::string>, std::vector<std::vector<int64_t>>, TMatrix<double>>;

    template<typename T>
    inline const T& value_getter(const Variant& value)
    {
      return std::get<T>(value);
    }

    template<>
    inline const bool& value_getter<bool>(const Variant& value)
    {
      return std::get<Bool>(value).m_Value;
    }

    template<TValueType v>
    struct Enum2type {
      enum { value = static_cast<uint8_t>(v) };
    };

    template<typename T>
    struct TypeForValueType {
      using Type = T;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::FLOATING_POINT>> {
      using Type = double;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::BOOLEAN>> {
      using Type = Bool;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::INTEGER>> {
      using Type = int64_t;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::ENUM>> {
      using Type = std::string;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::STRING>> {
      using Type = std::string;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::FILE_REFERENCE>> {
      using Type = std::string;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::BOOLEAN_ARRAY>> {
      using Type = std::vector<Bool>;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::FLOATING_POINT_ARRAY>> {
      using Type = std::vector<double>;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::INTEGER_ARRAY>> {
      using Type = std::vector<int64_t>;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::ENUM_ARRAY>> {
      using Type = std::vector<std::string>;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::REFERENCE_COMPONENT>> {
      using Type = int64_t;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::FLOATING_POINT_MATRIX>> {
      using Type = TMatrix<double>;
    };

    template<>
    struct TypeForValueType<Enum2type<TValueType::ARRAY_OF_INTEGER_ARRAYS>> {
      using Type = std::vector<std::vector<int64_t>>;
    };
  }

  using TFloatType = detail::TypeForValueType<detail::Enum2type<TValueType::FLOATING_POINT>>::Type;
  using TBoolType = detail::TypeForValueType<detail::Enum2type<TValueType::BOOLEAN>>::Type;
  using TIntType = detail::TypeForValueType<detail::Enum2type<TValueType::INTEGER>>::Type;
  using TEnumType = detail::TypeForValueType<detail::Enum2type<TValueType::ENUM>>::Type;
  using TStringType = detail::TypeForValueType<detail::Enum2type<TValueType::STRING>>::Type;
  using TFileReferenceType = detail::TypeForValueType<detail::Enum2type<TValueType::FILE_REFERENCE>>::Type;
  using TBoolArrayType = detail::TypeForValueType<detail::Enum2type<TValueType::BOOLEAN_ARRAY>>::Type;
  using TFloatArrayType = detail::TypeForValueType<detail::Enum2type<TValueType::FLOATING_POINT_ARRAY>>::Type;
  using TIntArrayType = detail::TypeForValueType<detail::Enum2type<TValueType::INTEGER_ARRAY>>::Type;
  using TEnumArrayType = detail::TypeForValueType<detail::Enum2type<TValueType::ENUM_ARRAY>>::Type;
  using TReferenceComponentType = detail::TypeForValueType<detail::Enum2type<TValueType::REFERENCE_COMPONENT>>::Type;
  using TFloatMatrixType = detail::TypeForValueType<detail::Enum2type<TValueType::FLOATING_POINT_MATRIX>>::Type;
  using TArrayOfIntArraysType = detail::TypeForValueType<detail::Enum2type<TValueType::ARRAY_OF_INTEGER_ARRAYS>>::Type;

  using FloatTag = detail::Enum2type<TValueType::FLOATING_POINT>;
  using IntTag = detail::Enum2type<TValueType::INTEGER>;
  using BoolTag = detail::Enum2type<TValueType::BOOLEAN>;
  using EnumTag = detail::Enum2type<TValueType::ENUM>;
  using StringTag = detail::Enum2type<TValueType::STRING>;
  using FileReferenceTag = detail::Enum2type<TValueType::FILE_REFERENCE>;
  using FloatArrayTag = detail::Enum2type<TValueType::FLOATING_POINT_ARRAY>;
  using BoolArrayTag = detail::Enum2type<TValueType::BOOLEAN_ARRAY>;
  using IntArrayTag = detail::Enum2type<TValueType::INTEGER_ARRAY>;
  using EnumArrayTag = detail::Enum2type<TValueType::ENUM_ARRAY>;
  using ReferenceComponentTag = detail::Enum2type<TValueType::REFERENCE_COMPONENT>;
  using FloatMatrinxTag = detail::Enum2type<TValueType::FLOATING_POINT_MATRIX>;
  using ArrayOfIntArraysTag = detail::Enum2type<TValueType::ARRAY_OF_INTEGER_ARRAYS>;
}

#endif
