#ifndef TPC_DETAIL_BSWAP_HPP
#define TPC_DETAIL_BSWAP_HPP

#ifdef __builtin_bswap32

    #define bswap32 __builtin_bswap32

#else

    #include <climits>

    template <typename T>
    T bswap(T u)
    {
        union
        {
            T u;
            unsigned char u8[sizeof(T)];
        } source, dest;

        source.u = u;

        for (size_t k = 0; k < sizeof(T); k++)
            dest.u8[k] = source.u8[sizeof(T) - k - 1];

        return dest.u;
    }

    #define bswap32 bswap

#endif

#endif // TPC_DETAIL_BSWAP_HPP
