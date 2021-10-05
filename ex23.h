#ifndef _EX23_H_
#define _EX23_H_

#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()
#define EXPAND(...) __VA_ARGS__

#define EVAL(...)  EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL3(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL4(...) EVAL5(EVAL5(EVAL5(__VA_ARGS__)))
#define EVAL5(...) __VA_ARGS__


#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define INC(x) PRIMITIVE_CAT(INC_, x)
#define INC_0 1
#define INC_1 2
#define INC_2 3
#define INC_3 4
#define INC_4 5
#define INC_5 6
#define INC_6 7
#define INC_7 8
#define INC_8 9
#define INC_9 9

#define DEC(x) PRIMITIVE_CAT(DEC_, x)
#define DEC_0 0
#define DEC_1 0
#define DEC_2 1
#define DEC_3 2
#define DEC_4 3
#define DEC_5 4
#define DEC_6 5
#define DEC_7 6
#define DEC_8 7
#define DEC_9 8
#define DEC_10 9
#define DEC_11 10
#define DEC_12 11
#define DEC_13 12
#define DEC_14 13
#define DEC_15 14
#define DEC_16 15
#define DEC_17 16
#define DEC_18 17
#define DEC_19 18
#define DEC_20 19
#define DEC_21 20
#define DEC_22 21
#define DEC_23 22
#define DEC_24 23
#define DEC_25 24
#define DEC_26 25
#define DEC_27 26
#define DEC_28 27
#define DEC_29 28
#define DEC_30 29
#define DEC_31 30
#define DEC_32 31
#define DEC_33 32
#define DEC_34 33
#define DEC_35 34
#define DEC_36 35
#define DEC_37 36
#define DEC_38 37
#define DEC_39 38
#define DEC_40 39
#define DEC_41 40
#define DEC_42 41
#define DEC_43 42
#define DEC_44 43
#define DEC_45 44
#define DEC_46 45
#define DEC_47 46
#define DEC_48 47
#define DEC_49 48
#define DEC_50 49
#define DEC_51 50
#define DEC_52 51
#define DEC_53 52
#define DEC_54 53
#define DEC_55 54
#define DEC_56 55
#define DEC_57 56
#define DEC_58 57
#define DEC_59 58
#define DEC_60 59
#define DEC_61 60
#define DEC_62 61
#define DEC_63 62
#define DEC_64 63
#define DEC_65 64

#define CHECK_N(x, n, ...) n
#define CHECK(...) CHECK_N(__VA_ARGS__, 0,)

#define NOT(x) CHECK(PRIMITIVE_CAT(NOT_, x))
#define NOT_0 ~, 1,

#define COMPL(b) PRIMITIVE_CAT(COMPL_, b)
#define COMPL_0 1
#define COMPL_1 0

#define BOOL(x) COMPL(NOT(x))

#define IIF(c) PRIMITIVE_CAT(IIF_, c)
#define IIF_0(t, ...) __VA_ARGS__
#define IIF_1(t, ...) t

#define IF(c) IIF(BOOL(c))

#define EAT(...)
#define EXPAND(...) __VA_ARGS__
#define WHEN(c) IF(c)(EXPAND, EAT)

#define REPEAT(count, macro, ...) \
    WHEN(count) \
    ( \
        OBSTRUCT(REPEAT_INDIRECT) () \
        ( \
            DEC(count), macro, __VA_ARGS__ \
        ) \
        OBSTRUCT(macro) \
        ( \
            DEC(count), __VA_ARGS__ \
        ) \
    )
#define REPEAT_INDIRECT() REPEAT

//An example of using this macro
#define M(i, _) i jopa DEC(i)

#define DUFF_REP(n, max) case max-n: *to = *from++ ;

#define DUFF(n) int wikiduff_device_ ## n (char *from, char *to, int count) { \
    int N = (count + n - 1) / n;  \
    switch (count % n) {          \
        do {                      \
    case 0: *to = *from++;        \
    EVAL(REPEAT(n, DUFF_REP, n))  \
            } while (--N > 0);    \
    }                             \
    return count;                 \
}

#endif

