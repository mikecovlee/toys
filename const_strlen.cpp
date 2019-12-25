// Template Metaprogramming Implementation
template<const char *str, int size, char ch>
struct strlen_helper : public strlen_helper<str, size + 1, str[size]> {
};

template<const char *str, int size>
struct strlen_helper<str, size, '\0'> {
    static constexpr int len = size;
};

template<const char *str> using const_strlen = strlen_helper<str, 0, *str>;

constexpr char str[] = "Hello, world!";
constexpr int len_tmp = const_strlen<str>::len;

// Constexpr Function Implementation
// Available in C++14 or above
constexpr int strlen(const char *str) {
    int count = 0;
    for (int i = 0; str[i] != '\0'; ++i)
        ++count;
    return count;
}

constexpr int len_fun = strlen("Hello");
