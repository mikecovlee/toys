#include "delegate.hpp"
#include <cstring>
#include <cstdio>

struct X {
    int x;
    int y;
    char s[32];
    double d;
};

int main()
{
    free(malloc(22));
    auto mx = static_cast<cov::byte_t *>(malloc(sizeof(int)));
    free(malloc(200));
    auto my = static_cast<cov::byte_t *>(malloc(sizeof(int)));
    free(malloc(20));
    auto ms = static_cast<cov::byte_t *>(malloc(sizeof(char[32])));
    free(malloc(112));
    auto md = static_cast<cov::byte_t *>(malloc(sizeof(double)));

    printf("mx: %p\n", mx);
    printf("my: %p\n", my);
    printf("ms: %p\n", ms);
    printf("md: %p\n", md);

    auto field=cov_make_field(cov_bind_member(X, x, mx), cov_bind_member(X, y, my), cov_bind_member(X, s, ms), cov_bind_member(X, d, md));

    cov_access_field(X, x, field)=10;
    cov_access_field(X, y, field)=20;
    strcpy(cov_access_field(X, s, field), "Hello,World");
    cov_access_field(X, d, field)=3.14;

    printf("x: %i\n", cov_access_field(X, x, field));
    printf("y: %i\n", cov_access_field(X, y, field));
    printf("s: %s\n", cov_access_field(X, s, field));
    printf("d: %f\n", cov_access_field(X, d, field));

    free(mx);
    free(my);
    free(ms);
    free(md);
    
    return 0;
}