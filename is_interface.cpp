// Traits Implement
template <template <typename> class TInterface, typename TImpl>
struct IsInterfaceTraits final
{
  static constexpr bool result = false;
};
template <template <typename> class TInterface, typename TImpl>
struct IsInterfaceTraits<TInterface, TInterface<TImpl>> final
{
  static constexpr bool result = true;
};
// SFINAE Implement
template <typename T>
T declval() {}

template <template <typename> class TInterface, typename TImpl>
class IsInterfaceSfinae final
{
  template <typename Impl>
  static void test(TInterface<Impl>) {}

  template <typename Impl>
  static constexpr bool match(...) { return false; }

  template <typename Impl, typename = decltype(test(declval<Impl>()))>
  static constexpr bool match(int) { return true; }

public:
  static constexpr bool result = match<TImpl>(0);
};

template <typename>
class test_class_1
{
};

template <typename>
class test_class_2
{
};

int main()
{
  // Traits
  static_assert(IsInterfaceTraits<test_class_1, test_class_1<int>>::result, "Failed!!!");
  static_assert(IsInterfaceTraits<test_class_1, test_class_2<int>>::result, "OK!!!");
  static_assert(IsInterfaceTraits<test_class_1, int>::result, "OK!!!");
  // SFINAE
  static_assert(IsInterfaceSfinae<test_class_1, test_class_1<int>>::result, "Failed!!!");
  static_assert(IsInterfaceSfinae<test_class_1, test_class_2<int>>::result, "OK!!!");
  static_assert(IsInterfaceSfinae<test_class_1, int>::result, "OK!!!");
}