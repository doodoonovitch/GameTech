#ifndef INTROSPECTION_TYPELIST_H
#define INTROSPECTION_TYPELIST_H

#pragma once

////////////////////////////////////////////////////////////////////////////////
// macros TYPELIST_1, TYPELIST_2, ... TYPELIST_50
// Each takes a number of arguments equal to its numeric suffix
// The arguments are type names. TYPELIST_NN generates a typelist containing 
//     all types passed as arguments, in that order.
// Example: TYPELIST_2(char, int) generates a type containing char and int.
////////////////////////////////////////////////////////////////////////////////

#define TYPELIST_1(T1) ::Introspection::Typelist<T1, ::Introspection::NullType>

#define TYPELIST_2(T1, T2) ::Introspection::Typelist<T1, TYPELIST_1(T2) >

#define TYPELIST_3(T1, T2, T3) ::Introspection::Typelist<T1, TYPELIST_2(T2, T3) >

#define TYPELIST_4(T1, T2, T3, T4) \
    ::Introspection::Typelist<T1, TYPELIST_3(T2, T3, T4) >

#define TYPELIST_5(T1, T2, T3, T4, T5) \
    ::Introspection::Typelist<T1, TYPELIST_4(T2, T3, T4, T5) >

#define TYPELIST_6(T1, T2, T3, T4, T5, T6) \
    ::Introspection::Typelist<T1, TYPELIST_5(T2, T3, T4, T5, T6) >

#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) \
    ::Introspection::Typelist<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7) >

#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) \
    ::Introspection::Typelist<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8) >

#define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) \
    ::Introspection::Typelist<T1, TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9) >

#define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) \
    ::Introspection::Typelist<T1, TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10) >

#define TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) \
    ::Introspection::Typelist<T1, TYPELIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) >

#define TYPELIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) \
    ::Introspection::Typelist<T1, TYPELIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12) >

#define TYPELIST_13(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) \
    ::Introspection::Typelist<T1, TYPELIST_12(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13) >

#define TYPELIST_14(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14) \
    ::Introspection::Typelist<T1, TYPELIST_13(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14) >

#define TYPELIST_15(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15) \
    ::Introspection::Typelist<T1, TYPELIST_14(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15) >

#define TYPELIST_16(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16) \
    ::Introspection::Typelist<T1, TYPELIST_15(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16) >

#define TYPELIST_17(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17) \
    ::Introspection::Typelist<T1, TYPELIST_16(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17) >

#define TYPELIST_18(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18) \
    ::Introspection::Typelist<T1, TYPELIST_17(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18) >

#define TYPELIST_19(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19) \
    ::Introspection::Typelist<T1, TYPELIST_18(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19) >

#define TYPELIST_20(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) \
    ::Introspection::Typelist<T1, TYPELIST_19(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) >

#define TYPELIST_21(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21) \
    ::Introspection::Typelist<T1, TYPELIST_20(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21) >

#define TYPELIST_22(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22) \
    ::Introspection::Typelist<T1, TYPELIST_21(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22) >

#define TYPELIST_23(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23) \
    ::Introspection::Typelist<T1, TYPELIST_22(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23) >

#define TYPELIST_24(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24) \
    ::Introspection::Typelist<T1, TYPELIST_23(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24) >

#define TYPELIST_25(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25) \
    ::Introspection::Typelist<T1, TYPELIST_24(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25) >

#define TYPELIST_26(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26) \
    ::Introspection::Typelist<T1, TYPELIST_25(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26) >

#define TYPELIST_27(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27) \
    ::Introspection::Typelist<T1, TYPELIST_26(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27) >

#define TYPELIST_28(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28) \
    ::Introspection::Typelist<T1, TYPELIST_27(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28) >

#define TYPELIST_29(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29) \
    ::Introspection::Typelist<T1, TYPELIST_28(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29) >

#define TYPELIST_30(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30) \
    ::Introspection::Typelist<T1, TYPELIST_29(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30) >

#define TYPELIST_31(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31) \
    ::Introspection::Typelist<T1, TYPELIST_30(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31) >

#define TYPELIST_32(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32) \
    ::Introspection::Typelist<T1, TYPELIST_31(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32) >

#define TYPELIST_33(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33) \
    ::Introspection::Typelist<T1, TYPELIST_32(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33) >

#define TYPELIST_34(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33, T34) \
    ::Introspection::Typelist<T1, TYPELIST_33(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33, T34) >

#define TYPELIST_35(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35) \
    ::Introspection::Typelist<T1, TYPELIST_34(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35) >

#define TYPELIST_36(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36) \
    ::Introspection::Typelist<T1, TYPELIST_35(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36) >

#define TYPELIST_37(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37) \
    ::Introspection::Typelist<T1, TYPELIST_36(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37) >

#define TYPELIST_38(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38) \
    ::Introspection::Typelist<T1, TYPELIST_37(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38) >

#define TYPELIST_39(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39) \
    ::Introspection::Typelist<T1, TYPELIST_38(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39) >

#define TYPELIST_40(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40) \
    ::Introspection::Typelist<T1, TYPELIST_39(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40) >

#define TYPELIST_41(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41) \
    ::Introspection::Typelist<T1, TYPELIST_40(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41) >

#define TYPELIST_42(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42) \
    ::Introspection::Typelist<T1, TYPELIST_41(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42) >

#define TYPELIST_43(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43) \
    ::Introspection::Typelist<T1, TYPELIST_42(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43) >

#define TYPELIST_44(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43, T44) \
    ::Introspection::Typelist<T1, TYPELIST_43(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43, T44) >

#define TYPELIST_45(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45) \
    ::Introspection::Typelist<T1, TYPELIST_44(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45) >

#define TYPELIST_46(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46) \
    ::Introspection::Typelist<T1, TYPELIST_45(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46) >

#define TYPELIST_47(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47) \
    ::Introspection::Typelist<T1, TYPELIST_46(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47) >

#define TYPELIST_48(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48) \
    ::Introspection::Typelist<T1, TYPELIST_47(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48) >

#define TYPELIST_49(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48, T49) \
    ::Introspection::Typelist<T1, TYPELIST_48(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48, T49) >

#define TYPELIST_50(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48, T49, T50) \
    ::Introspection::Typelist<T1, TYPELIST_49(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48, T49, T50) >




////////////////////////////////////////////////////////////////////////////////
// macro STATIC_CHECK
// Invocation: STATIC_CHECK(expr, id)
// where:
// expr is a compile-time integral or pointer expression
// id is a C++ identifier that does not need to be defined
// If expr is zero, id will appear in a compile-time error message.
////////////////////////////////////////////////////////////////////////////////

#define STATIC_CHECK(expr, msg) typedef char ERROR_##msg[1][(expr)]


////////////////////////////////////////////////////////////////////////////////
//
// this macro will cause compile time error if _type_ is not a Typelist or NullType
//
////////////////////////////////////////////////////////////////////////////////
#define ASSERT_TYPELIST(_type_) STATIC_CHECK( ::Introspection::TL_is_Typelist<_type_>::value, TList_is_not_legal_Typelist )



namespace  Introspection
{


////////////////////////////////////////////////////////////////////////////////
// class template Int2Type
// Converts each integral constant into a unique type
// Invocation: Int2Type<v> where v is a compile-time constant integral
// Defines 'value', an enum that evaluates to v
////////////////////////////////////////////////////////////////////////////////

template <int v>
struct Int2Type
{
	enum { value = v };
};

////////////////////////////////////////////////////////////////////////////////
// class template Type2Type
// Converts each type into a unique, insipid type
// Invocation Type2Type<T> where T is a type
// Defines the type OriginalType which maps back to T
////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Type2Type
{
	typedef T OriginalType;
	//Type2Type() {} // VC7
};



////////////////////////////////////////////////////////////////////////////////
// class NullType
// Used as a placeholder for "no type here"
// Useful as an end marker in typelists 
////////////////////////////////////////////////////////////////////////////////
class NullType
{
public:
	struct Head { private: Head(); };
	struct Tail { private: Tail(); };
};



////////////////////////////////////////////////////////////////////////////////
// class template Typelist
// The building block of typelists of any length
// Use it through the TYPELIST_NN macros
// Defines nested types:
//     Head (first element, a non-typelist type by convention)
//     Tail (second element, can be another typelist)
////////////////////////////////////////////////////////////////////////////////
template <typename T, typename U>
struct Typelist
{
	typedef T Head;
	typedef U Tail;
};



////////////////////////////////////////////////////////////////////////////////
// class template is_Typelist
// detects if type is Typelist (including Nulltype)
// Invocation :
// is_Typelist<T>::value
// returns a compile-time boolean constant containing true iff T is some Typelist<T1,T2>
// is_Typelist<T>::type_id
// returns a compile-time unsigned constant containing 
// 1 iff T == Typelist<T1,T2>, 2 iff T == NullType and 3 otherwise
////////////////////////////////////////////////////////////////////////////////
struct Typelist_tag {};
struct NullType_tag {};
struct NoneList_tag {};

enum
{
	Typelist_ID = 1,
	NullType_ID = 2,
	NoneList_ID = 3
};

template<typename T>
struct TL_is_Typelist
{
private:
	typedef char(&ye1)[Typelist_ID];
	typedef char(&ye2)[NullType_ID];
	typedef char(&no)[NoneList_ID];

	template<typename Head, typename Tail>
	static ye1 check(Type2Type< Typelist<Head, Tail> >);
	static ye2 check(Type2Type<NullType>);

	static no  check(...);

public:
#if (_MSC_VER >= 1300)
	// VC7 fail NPS_HierarchyGenerators.h if this one is enum
	static const unsigned int type_id = sizeof(check(Type2Type<T>()));
#else
	enum { type_id = sizeof(check(Type2Type<T>())) };
#endif

	enum { value = type_id != sizeof(no) };

	typedef typename std::conditional
	<
		type_id == Typelist_ID,
		Typelist_tag,
		typename std::conditional<type_id == NullType_ID, NullType_tag, NoneList_tag>::type
	>
	::type type_tag;
};



////////////////////////////////////////////////////////////////////////////////
// class template TL_Length
// Computes the length of a typelist
// Invocation (TList is a typelist):
// TL_Length<TList>::value
// returns a compile-time constant containing the length of TList, not counting
//     the end terminator (which by convention is NullType)
////////////////////////////////////////////////////////////////////////////////
template <typename TList> struct TL_Length;

template <>
struct TL_Length<NullType>
{
	enum { value = 0 };
};

template <typename TList>
struct TL_Length
{
private:
	ASSERT_TYPELIST(TList);

	typedef typename TList::Head Head;
	typedef typename TList::Tail Tail;

public:
	enum { value = 1 + TL_Length<Tail>::value };
};



////////////////////////////////////////////////////////////////////////////////
// class template TL_TypeAt
// Finds the type at a given index in a typelist
// Invocation (TList is a typelist and index is a compile-time integral 
//     constant):
// TL_TypeAt<TList, index>::Result
// returns the type in position 'index' in TList
// If you pass an out-of-bounds index, the result is a compile-time error
////////////////////////////////////////////////////////////////////////////////
template <typename TList, unsigned int index>
struct TL_TypeAt
{
	typedef typename TList::Head Head;
	typedef typename TList::Tail Tail;

private:
	ASSERT_TYPELIST(TList);

	template<unsigned int i>
	struct In
	{
		typedef typename TL_TypeAt<Tail, i - 1>::Result Result;
	};

	template<>
	struct In<0>
	{
		typedef Head Result;
	};

public:
	typedef typename In<index>::Result Result;
};

////////////////////////////////////////////////////////////////////////////////
// class template TL_TypeAtNonStrict
// Finds the type at a given index in a typelist
// Invocations (TList is a typelist and index is a compile-time integral 
//     constant):
// a) TL_TypeAtNonStrict<TList, index>::Result
// returns the type in position 'index' in TList, or NullType if index is 
//     out-of-bounds
// b) TL_TypeAtNonStrict<TList, index, D>::Result
// returns the type in position 'index' in TList, or D if index is out-of-bounds
////////////////////////////////////////////////////////////////////////////////
template <typename TList, unsigned int index, typename DefaultType = NullType>
struct TL_TypeAtNonStrict
{
	typedef typename TList::Head Head;
	typedef typename TList::Tail Tail;

private:
	ASSERT_TYPELIST(TList);

	template<class TList1, unsigned int i>
	struct In
	{
		typedef typename TL_TypeAtNonStrict<typename TList1::Tail, i - 1, DefaultType>::Result Result;
	};

	template<>
	struct In<Typelist<Head, Tail>, 0>
	{
		typedef Head Result;
	};

	template<>
	struct In<NullType, index>
	{
		typedef DefaultType Result;
	};
public:
	typedef typename In<TList, index>::Result Result;
};



////////////////////////////////////////////////////////////////////////////////
// class template TL_Append
// Appends a type or a typelist to another
// Invocation (TList is a typelist and T is either a type or a typelist):
// TL_Append<TList, T>::Result
// returns a typelist that is TList followed by T and NullType-terminated
////////////////////////////////////////////////////////////////////////////////

template <typename TList, typename T>
struct TL_Append
{
	typedef typename TList::Head Head;
	typedef typename TList::Tail Tail;

private:
	ASSERT_TYPELIST(TList);

	template<typename TList1>
	struct In
	{
		typedef typename TList1::Tail Tail;

		typedef Typelist<Head, typename TL_Append<Tail, T>::Result> Result;
	};

	template<>
	struct In<NullType>
	{
		typedef typename std::conditional<TL_is_Typelist<T>::value, T, TYPELIST_1(T)>::type Result;
	};

public:
	typedef typename In<TList>::Result Result;
};



////////////////////////////////////////////////////////////////////////////////
// class template TL_IndexOf
// Finds the index of a type in a typelist
// Invocation (TList is a typelist and T is a type):
// TL_IndexOf<TList, T>::value
// returns the position of T in TList, or NullType if T is not found in TList
////////////////////////////////////////////////////////////////////////////////

template <class TList, class T>
struct TL_IndexOf
{
	typedef typename TList::Head Head;
	typedef typename TList::Tail Tail;

private:
	ASSERT_TYPELIST(TList);

	template<class TList1>
	struct In
	{
	private:
		typedef typename TList1::Tail Tail;

		enum { temp = (TL_IndexOf<Tail, T>::value) };

	public:
		enum { value = temp == -1 ? -1 : 1 + temp };
	};

	template<>
	struct In< Typelist<T, Tail> >
	{
		enum { value = 0 };
	};

	template<>
	struct In<NullType>
	{
		enum { value = -1 };
	};

public:
	enum { value = In<TList>::value };
};



////////////////////////////////////////////////////////////////////////////////
// class template TL_Contains
// Finds if the typelist contains the type T
// Invocation (TList is a typelist and T is a type):
// TL_Contains<TList, T>::value
// returns true if TList contains T, false or else.
////////////////////////////////////////////////////////////////////////////////

template <class TList, class T>
struct TL_Contains
{
	typedef typename TList::Head Head;
	typedef typename TList::Tail Tail;

private:
	ASSERT_TYPELIST(TList);

	template<class TList1>
	struct In
	{
	private:
		typedef typename TList1::Tail Tail;

	public:
		enum { value = TL_Contains<Tail, T>::value };
	};

	template<>
	struct In< Typelist<T, Tail> >
	{
		enum { value = true };
	};

	template<>
	struct In<NullType>
	{
		enum { value = false };
	};

public:
	enum { value = In<TList>::value };
};



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



typedef TYPELIST_19(
	bool, char, wchar_t, float, double, // 5
	int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, // 8
	std::string, std::wstring,
	char*, wchar_t*, const char*, const wchar_t*
	) BasicTypeList;



/*
bool*, char*, wchar_t*, float*, double*, // 5
int8_t*, uint8_t*, int16_t*, uint16_t*, int32_t*, uint32_t*, int64_t*, uint64_t*, // 8
std::string*, std::wstring*
*/

} // namespace  Introspection
#endif // INTROSPECTION_TYPELIST_H