
// MyEuNumber for C

#pragma once

#ifndef myEuNumber_h__
#define myEuNumber_h__

#define FALSE 0
#define TRUE 1

// Define "unix" on Linux and Unix-like systems, example: macOS or OSX (Historical Note: Apple's "OSX" came before Apple's "macOS")

#ifdef unix
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define sprintf_s(a,b,c,d) sprintf(a,c,d)
#endif

// Configure "MY_API" macro for your compiler:

// If using the DLL, Comment out this line:
//#define MY_API

#ifndef MY_API

// If using or compiling the DLL:
// If Microsoft: Define "MS" when compiling for Microsoft, and optionally "MYEXPORT" when compiling for the DLL
#ifdef MS
#ifdef MYEXPORT
#define MY_API __declspec(dllexport)
#else
#define MY_API __declspec(dllimport)
#endif
#else
// For GCC: Uncomment next line for* .so Linux, or* .dylib MacOS (really osx)
#define MY_API extern
#endif

#endif

// For 64-bit integers,
// maximum values for "targetLength <=> suggested length value for significant digits"
// and "radix <=> base, which must be 2 (two) or greater"

// Define this on old X86, or 32-bit, platforms, forces the use of 32-bit integers:
//#define BITS32

#ifndef BITS32
#ifndef BITS64
#define BITS64
#endif
#endif

#ifdef BITS64
#define EUN_INT_MAX 4611686018427387903LL
#define EUN_MAX_RADIX 134217728LL
#define EUN_INT_MAX10 1000000000000000000LL
#define EUN_MAX_RADIX10 100000000LL
#else
#define EUN_INT_MAX 1073741823L
#define EUN_MAX_RADIX 2048L
#define EUN_INT_MAX10 1000000000L
#define EUN_MAX_RADIX10 1000L
#endif

// Flag:
#define MY_POS_INFINITY 1
#define MY_NEG_INFINITY 2
#define MY_IMAGINARY 4

// Rounding: roundingMethod
// round towards +infinity or -infinity:
#define ROUND_INF 1
// round towards zero:
#define ROUND_ZERO 2
// round towards positive +infinity
#define ROUND_POS_INF 4
// round towards negative -infinity
#define ROUND_NEG_INF 5
// round up or down:
#define ROUND_DOWN 5
#define ROUND_UP 4
// round towards even or odd:
#define ROUND_EVEN 6
#define ROUND_ODD 7

#define ROUND_AWAY_FROM_ZERO 1
#define ROUND_TOWARDS_ZERO 2
#define ROUND_TOWARDS_POSITIVE_INFINITY 4
#define ROUND_TOWARDS_NEGATIVE_INFINITY 5

#ifndef FLOAT80
#ifndef FLOAT64
#define FLOAT64
#endif // !FLOAT64
#endif // !FLOAT80

#ifdef FLOAT80
typedef long double Float;
#else
#ifdef FLOAT64
typedef double Float;
#else
// Try not to use "float", it may have errors on larger radixes.
typedef float Float;
#endif
#endif

#ifdef BITS64
typedef long long Integer; // NOTE: 64-bit signed integer
typedef unsigned long long UnsignedInteger; // NOTE: 64-bit signed integer
#else
typedef int Integer; // NOTE: 32-bit signed integer
typedef unsigned int UnsignedInteger; // NOTE: 32-bit signed integer
#endif

typedef struct integerArray_1 {
	Integer len;
	Integer* array;
} IntegerArray;

typedef struct datatype_1 {
	unsigned long long version;
	struct {
		unsigned long long numberOfIntegerBits : 16; // 255bit or lower
		unsigned long long numberOfPointerBits : 16; // 255bit or lower
		unsigned long long numberOfFloatBits : 16; // 255bit or lower
		unsigned long long roundingMethod : 3;
		unsigned long long roundToNearestOption : 1;
		unsigned long long realMode : 1; // true or false, default is false.
		unsigned long long extended : 11; // might be used for later
	};
	void* (*myMalloc)(size_t);
	void (*myFree)(void*);
	Integer defaultTargetLength; // here Integer defaultTargetLength
	Integer defaultRadix;
	Float calcSpeed; // default is 1.0
	Integer lastIterCount;
	Integer lastNthRootIter;
	void (*myErrorCallBack)(long long);
	Integer adjustRound;
	Integer multInvMoreAccuracy;
	Integer nthRootMoreAccuracy;
} EunInstanceData;

typedef struct eun_1 {
	EunInstanceData* data;
	IntegerArray* num; // array of integers
	Integer exp;
	Integer targetLength;
	Integer radix;
	union {
		unsigned int flags;
		struct {
			unsigned int infinityFlag : 2; // tells you if +inf or -inf
			unsigned int imaginaryFlag : 1;
			unsigned int divideByZeroFlag : 1;
			unsigned int nanFlag : 1; // nan, or "not a number" flag
			//unsigned int reserved : 27;
		};
	};
} Eun;

typedef struct expToAtomRet_1 {
	Float a;
	Integer i;
} ExpToAtomRet;

typedef struct eun_args_1 {
	Integer count;
	Eun** values;
} EunRetValues;

typedef struct complex_1 {
	Eun* real;
	Eun* imag;
} Complex;

typedef struct complex_args_1 {
	Integer count;
	Complex** values;
} ComplexRetValues;

typedef struct quad_ret_value_1 {
	Integer isComplex;
	union quad_union_1 {
		ComplexRetValues* complexValues;
		EunRetValues* eunValues;
	} values;
} QuadraticEquationRet;


// Function prototypes:

#ifdef __cplusplus
extern "C" {
#endif

	// C exported functions:

	MY_API int EunVersion(); // call this at the beginning of your program to check version.
	MY_API int NumberOfPointerBits();
	MY_API int NumberOfIntegerBits();
	MY_API int NumberOfFloatBits();
	MY_API int SizeOfInstance(); // sizeof(EunInstanceData)

	MY_API void EunInit();
	MY_API void DefaultErrorCallBack(long long i);
	MY_API Integer EunInitInstance(EunInstanceData* ret, Integer dataTypeSize, void* (*newMalloc)(size_t), void (*newFree)(void*), void (*newErrorCallBack)(long long));
	MY_API Integer GetDivideByZeroFlag(Eun a);
	MY_API void SetDefaultTargetLength(EunInstanceData d, Integer i);
	MY_API Integer GetDefaultTargetLength(EunInstanceData d);
	MY_API void SetDefaultRadix(EunInstanceData d, Integer i);
	MY_API Integer GetDefaultRadix(EunInstanceData d);
	MY_API void SetAdjustRound(EunInstanceData d, Integer i);
	MY_API Integer GetAdjustRound(EunInstanceData d);
	MY_API void SetCalcSpeed(EunInstanceData d, Float speed);
	MY_API Float GetCalcSpeed(EunInstanceData d);
	MY_API void SetMultInvMoreAccuracy(EunInstanceData d, Integer i);
	MY_API Integer GetMultInvMoreAccuracy(EunInstanceData d);
	MY_API void SetRound(EunInstanceData d, Integer i);
	MY_API Integer GetRound(EunInstanceData d);
	MY_API void SetRoundToNearestOption(EunInstanceData d, Integer boolean_value_num);
	MY_API Integer GetRoundToNearestOption(EunInstanceData d);
	MY_API void SetRoundEx(EunInstanceData* ret, Integer roundingMethod, Integer roundToNearestOption);
	MY_API void SetRealMode(EunInstanceData* ret, Integer enableRealMode); // this is either true (1) or false (0)

	MY_API void ErrorMessageNumberOutOfRange();
	MY_API void ErrorMessageNumberOverflow();
	MY_API void ErrorMessageRanOutOfMemory();

	// Language specific functions:

	MY_API IntegerArray* NewIntegerArray(EunInstanceData* d, Integer len);
	MY_API IntegerArray* IntegerArrayFromIntegers(EunInstanceData* d, const Integer* a, Integer len);
	MY_API IntegerArray* CopyIntegerArray(EunInstanceData* d, const IntegerArray* src);
	MY_API void PushFrontIntegerArray(EunInstanceData* d, IntegerArray* src, Integer digit); // prepend() integerArray
	MY_API void PopFrontIntegerArray(EunInstanceData* d, IntegerArray* src, size_t num);
	MY_API void PopBackIntegerArray(EunInstanceData* d, IntegerArray* src, size_t num);
	MY_API void ResizeIntegerArray(EunInstanceData* d, IntegerArray* src, Integer len); // here
	MY_API void FreeIntegerArray(EunInstanceData* d, IntegerArray* a);

	MY_API Eun* NewEuNumber(EunInstanceData* d, IntegerArray* n1, Integer exp1, Integer targetLength, Integer radix, unsigned int flags);
	MY_API Eun* CopyEun(const Eun* a);
	MY_API void FreeEuNumber(Eun* a);

	MY_API Float RoundTowardsZero(Float x);
	MY_API Integer ArraysEqual(const IntegerArray* a, const IntegerArray* b);
	MY_API Integer RangeArraysEqual(const IntegerArray* a, const IntegerArray* b, Integer start, Integer stop);

#define IsIntegerOdd(T) ((T) & 1)
#define IsIntegerEven(T) ((T) & 0)

	// Start of myEuNumber functions:

	MY_API void Borrow(IntegerArray* num, Integer radix);
	MY_API void NegativeBorrow(IntegerArray* num, Integer radix);
	MY_API void Carry(EunInstanceData* d, IntegerArray* num, Integer radix);
	MY_API void NegativeCarry(EunInstanceData* d, IntegerArray* num, Integer radix);
	MY_API void CarryRadixOnly(EunInstanceData* d, IntegerArray* num, Integer radix);
	MY_API void NegativeCarryRadixOnly(EunInstanceData* d, IntegerArray* num, Integer radix);
	//MY_API void AddTo(EunInstanceData* d, IntegerArray** addTo, IntegerArray** addFrom);
	MY_API IntegerArray* Add(EunInstanceData* d, const IntegerArray* n1, const IntegerArray* n2, Integer isSubtraction);
	MY_API IntegerArray* ConvertRadix(EunInstanceData* d, const IntegerArray* number, Integer fromRadix, Integer toRadix);
	MY_API IntegerArray* Mult(EunInstanceData* d, const IntegerArray* n1, const IntegerArray* n2);

	MY_API int IsNegative(const IntegerArray* a);
	MY_API void Negate(IntegerArray* toNegate);
	MY_API void AbsoluteValue(IntegerArray* toNegate);
	MY_API void Subtract(EunInstanceData* d, IntegerArray* num, Integer radix, Integer isMixed);
	MY_API void TrimLeadingZeros(EunInstanceData* d, IntegerArray* num);
	MY_API void TrimTrailingZeros(EunInstanceData* d, IntegerArray* num);

	MY_API void AdjustRound(Eun* n1, Integer isMixed);

	MY_API Eun* MultExp(EunInstanceData* d, const IntegerArray* n1, Integer exp1, const IntegerArray* n2, Integer exp2, Integer targetLength, Integer radix);
	MY_API Eun* AddExp(EunInstanceData* d, const IntegerArray* n1, Integer exp1, const IntegerArray* n2, Integer exp2, Integer targetLength, Integer radix, Integer isSubtraction);
	MY_API Eun* SubtractExp(EunInstanceData* d, const IntegerArray* n1, Integer exp1, const IntegerArray* n2, Integer exp2, Integer targetLength, Integer radix);

	// Division and Mult Inverse:
	MY_API Eun* ProtoMultInvExp(EunInstanceData* d, const IntegerArray* n1, Integer exp1, const IntegerArray* n2, Integer exp2, Integer targetLength, Integer radix);
	MY_API IntegerArray* IntToDigits(EunInstanceData* d, Float x, Integer radix);
	MY_API ExpToAtomRet* ExpToAtom(EunInstanceData* d, const IntegerArray* n1, Integer exp1, Integer targetLength, Integer radix);
	MY_API Eun* GetGuessExp(EunInstanceData* d, const IntegerArray* den, Integer exp1, Integer protoMax, Integer radix);

	MY_API Eun* MultInvExp(EunInstanceData* d, const IntegerArray* den1, Integer exp1, Integer targetLength, Integer radix);
	MY_API Eun* DivideExp(EunInstanceData* d, const IntegerArray* n1, Integer exp1, const IntegerArray* n2, Integer exp2, Integer targetLength, Integer radix);
	MY_API Eun* ConvertExp(EunInstanceData* d, const IntegerArray* n1, Integer exp1, Integer targetLength, Integer fromRadix, Integer toRadix);

	// Eun functions:

	MY_API void EunPrint(const Eun* n1);
	MY_API void EunAdjustRound(Eun* n1, Integer adjustBy);
	MY_API Eun* EunMult(const Eun* n1, const Eun* n2);
	MY_API Eun* EunAdd(const Eun* n1, const Eun* n2);
	MY_API int EunIsNegative(const Eun* n1);
	MY_API Eun* EunNegate(const Eun* n1);
	MY_API Eun* EunSubtract(const Eun* n1, const Eun* n2);
	MY_API Eun* EunMultInv(const Eun* n1);
	MY_API Eun* EunDivide(const Eun* n1, const Eun* n2);
	MY_API Eun* EunConvert(const Eun* n1, Integer to_radix, Integer targetLength);

	MY_API Integer CompareExp(IntegerArray* n1, Integer exp1, IntegerArray* n2, Integer exp2);
	MY_API Integer EunCompare(const Eun* n1, const Eun* n2);
	MY_API IntegerArray* ReverseIntegerArray(EunInstanceData* d, const IntegerArray* s);
	MY_API Eun* EunReverse(const Eun* n1);
	MY_API Eun* EunFracPart(const Eun* n1);
	MY_API Eun* EunIntPart(const Eun* n1);
	MY_API void EunRoundSig(Eun* n1, Integer sigDigits);
	MY_API void EunRoundSignificantDigits(Eun* n1, Integer sigDigits);
	MY_API void EunRoundToInt(Eun* n1);
	MY_API Eun* EunCombInt(const Eun* in1, Integer adjustBy);

	MY_API char* AllocateString(EunInstanceData* d, const char* in);

	// StringToEun() is the prefered over ToEun() for creating of MyEuNumbers.
	MY_API Eun* StringToEun(EunInstanceData* d, const char* buf, Integer radix, Integer targetLength);
	MY_API Eun* ToEun(EunInstanceData* d, Float a, Integer radix, Integer targetLength);
	MY_API char* ToString(const Eun* a);
	MY_API Float ToAtom(const Eun* a);

	// added functions:

	MY_API Eun* IntPowerExp(EunInstanceData* d, Integer to_power, const IntegerArray* n1, Integer exp1, Integer targetLength, Integer radix);
	MY_API Eun* NthRootProtoExp(EunInstanceData* d, Integer n, const IntegerArray* x1, Integer x1Exp, const IntegerArray* guess, Integer guessExp, Integer targetLength, Integer radix);
	MY_API Eun* NthRootExp(EunInstanceData* d, Integer n, const IntegerArray* x1, Integer x1Exp, const IntegerArray* myguess, Integer guessExp, Integer targetLength, Integer radix);

	MY_API EunRetValues* EunNthRootGuessExp(Integer n, const Eun* in1, const Eun* guess, Integer exp1);
	MY_API EunRetValues* EunNthRootGuess(Integer n, const Eun* in1, const Eun* guess);
	MY_API EunRetValues* EunNthRoot(Integer n, const Eun* in1);

	MY_API EunRetValues* EunSquareRoot(const Eun* n1);
	MY_API EunRetValues* EunCubeRoot(const Eun* n1);
	MY_API EunRetValues* EunSqrt(const Eun* n1);

	MY_API QuadraticEquationRet* EunQuadraticEquation(const Eun* a, const Eun* b, const Eun* c);

	// Complex Function prototypes:

	MY_API Complex* NewComplex(Eun* real, Eun* imag);
	MY_API void FreeComplex(Complex* a);
	MY_API void NegateImagPart(Complex* a);
	MY_API Complex* ComplexAdd(const Complex* a, const Complex* b);
	MY_API Complex* ComplexNegate(const Complex* b);
	MY_API Complex* ComplexSubtract(const Complex* a, const Complex* b);
	MY_API Complex* ComplexMult(const Complex* n1, const Complex* n2);
	MY_API Complex* ComplexMultInv(const Complex* n2);
	MY_API Complex* ComplexDivide(const Complex* n1, const Complex* n2);
	MY_API ComplexRetValues* ComplexSqrt(const Complex* a);
	MY_API ComplexRetValues* ComplexQuadraticEquation(const Complex* a, const Complex* b, const Complex* c);


#ifdef __cplusplus
}
#endif

#endif // myEuNumber_h__
