
// MyEuNumber for C

// Indexes start at 0 in C, and go to len-1.

// NOTE: Only write on memory areas that have been allocated.

#include "pch.h"
#include "myeunumber.h"

// C exported functions:

int EunVersion()
{
	return 41;
}

int NumberOfPointerBits()
{
	return sizeof(void*) << 3;
}

int NumberOfIntegerBits()
{
	return sizeof(Integer) << 3;
}

int NumberOfFloatBits()
{
	return sizeof(Float) << 3;
}

int SizeOfInstance()
{
	return sizeof(EunInstanceData);
}

Float logMin;
Float logMax;

Integer valueOfTwo[1] = { 2 };
IntegerArray valTwo;
Integer valueOfOne[1] = { 1 };
IntegerArray valOne;
Integer valueOfZero[1] = { 0 };
IntegerArray valZero;
Integer maxIter;

void EunInit()
{
	valTwo.len = 1;
	valTwo.array = (Integer*)&valueOfTwo;
	valOne.len = 1;
	valOne.array = (Integer*)&valueOfOne;
	valZero.len = 1;
	valZero.array = (Integer*)&valueOfZero;
#ifdef FLOAT80
	logMin = logl(LDBL_MIN);
	logMax = logl(LDBL_MAX);
#else
#ifdef FLOAT64
	logMin = log(DBL_MIN);
	logMax = log(DBL_MAX);
#else
	logMin = logf(FLT_MIN);
	logMax = logf(FLT_MAX);
#endif
#endif
	maxIter = EUN_INT_MAX10;
}

void DefaultErrorCallBack(long long i)
{
	printf("Error(%lli): ", i);
	switch (i)
	{
	case 1:
		printf("In MyEuNumber, tried to divide by zero (1/0).");
		break;
	case 2:
		printf("In MyEuNumber, even root of -1, i.e. sqrt(-1).");
		break;
	case 3:
		printf("In MyEuNumber, improperly formatted number.");
		break;
	case 4:
		printf("In MyEuNumber, in EunCompare, radixes (also known as bases) are not the same.");
		break;
	case 5:
		printf("In MyEuNumber, in ComplexSqrt, number should not be imaginary.");
		break;
	default:
		break;
	}
	printf("\n");
	exit(EXIT_FAILURE);
}

Integer EunInitInstance(EunInstanceData* ret, Integer dataTypeSize, void* (*newMalloc)(size_t), void (*newFree)(void*), void (*newErrorCallBack)(long long))
{
	if ((ret == NULL) || (dataTypeSize != sizeof(EunInstanceData)))
	{
		return 0; // return zero on error
	}
	ret->version = EunVersion();
	ret->numberOfIntegerBits = NumberOfIntegerBits();
	ret->numberOfPointerBits = NumberOfPointerBits();
	ret->numberOfFloatBits = NumberOfFloatBits();
	ret->roundingMethod = ROUND_INF;
	ret->roundToNearestOption = 0; // round to nearest integer
	ret->realMode = 0; // true (1) or false (0), default is false
	ret->extended = 0;
	if (newMalloc == NULL)
	{
		newMalloc = &malloc; // pointer to malloc function
	}
	ret->myMalloc = newMalloc;
	if (newFree == NULL)
	{
		newFree = &free; // pointer to free function
	}
	ret->myFree = newFree;
	ret->defaultTargetLength = 70; // bus size is 212; (70 * 3 = 210)
	ret->defaultRadix = 10; // New: default is now an odd radix.
#ifdef FLOAT80
	ret->calcSpeed = 1.0ld;
#else
#ifdef FLOAT64
	ret->calcSpeed = 1.0;
#else
	ret->calcSpeed = 1.0f;
#endif
#endif
	ret->lastIterCount = 0;
	ret->lastNthRootIter = 0;
	if (newErrorCallBack == NULL)
	{
		newErrorCallBack = &DefaultErrorCallBack;
	}
	ret->myErrorCallBack = newErrorCallBack;
	ret->adjustRound = 0;
	ret->multInvMoreAccuracy = 0;
	ret->nthRootMoreAccuracy = 0;
	return dataTypeSize; // successfully wrote data
}

Integer GetDivideByZeroFlag(Eun a)
{
	return a.divideByZeroFlag;
}

void SetDefaultTargetLength(EunInstanceData d, Integer i)
{
	d.defaultTargetLength = i;
}

Integer GetDefaultTargetLength(EunInstanceData d)
{
	return d.defaultTargetLength;
}

void SetDefaultRadix(EunInstanceData d, Integer i)
{
	d.defaultRadix = i;
}

Integer GetDefaultRadix(EunInstanceData d)
{
	return d.defaultRadix;
}

void SetAdjustRound(EunInstanceData d, Integer i)
{
	d.adjustRound = i;
}

Integer GetAdjustRound(EunInstanceData d)
{
	return d.adjustRound;
}

void SetCalcSpeed(EunInstanceData d, Float speed)
{
	d.calcSpeed = speed;
}

Float GetCalcSpeed(EunInstanceData d)
{
	return d.calcSpeed;
}

void SetMultInvMoreAccuracy(EunInstanceData d, Integer i)
{
	d.multInvMoreAccuracy = i;
}

Integer GetMultInvMoreAccuracy(EunInstanceData d)
{
	return d.multInvMoreAccuracy;
}

void SetRound(EunInstanceData d, Integer i)
{
	d.roundingMethod = i;
}

Integer GetRound(EunInstanceData d)
{
	return d.roundingMethod;
}

void SetRoundToNearestOption(EunInstanceData d, Integer boolean_value_num)
{
	d.roundToNearestOption = boolean_value_num;
}

Integer GetRoundToNearestOption(EunInstanceData d)
{
	return d.roundToNearestOption;
}

void SetRoundEx(EunInstanceData* ret, Integer roundingMethod, Integer roundToNearestOption)
{
	ret->roundingMethod = roundingMethod;
	ret->roundToNearestOption = roundToNearestOption;
}

void SetRealMode(EunInstanceData* ret, Integer enableRealMode)
{
	ret->realMode = enableRealMode; // true (1) or false (0)
}


void ErrorMessageNumberOutOfRange()
{
	puts("Error, a number is out of range.\n");
	exit(EXIT_FAILURE);
}

void ErrorMessageNumberOverflow()
{
	puts("Error, a integer value overflow.  Try using a smaller radix.\n");
	exit(EXIT_FAILURE);
}

void ErrorMessageRanOutOfMemory()
{
	puts("Error, program ran out of memory.\n");
	exit(EXIT_FAILURE); // ran out of memory
}

// new integerArray of len
IntegerArray* NewIntegerArray(EunInstanceData* d, Integer len)
{
	IntegerArray* ret;
	if (len < 0)
	{
		ErrorMessageNumberOutOfRange();
	}
	ret = (IntegerArray*)d->myMalloc(sizeof(IntegerArray));
	if (ret == NULL)
	{
		ErrorMessageRanOutOfMemory();
	}
	size_t size = (size_t)(len * sizeof(Integer));
	ret->array = (Integer*)d->myMalloc(size);
	if (ret->array == NULL)
	{
		ErrorMessageRanOutOfMemory();
	}
	memset(ret->array, 0, size); // needed.
	ret->len = len;
	return ret;
}

// integerArray from int array
IntegerArray* IntegerArrayFromIntegers(EunInstanceData* d, const Integer* a, Integer len)
{
	IntegerArray* ret;
	ret = NewIntegerArray(d, len);
	memcpy(ret->array, a, (size_t)(len * sizeof(Integer)));
	return ret;
}

// copy integerArray
IntegerArray* CopyIntegerArray(EunInstanceData* d, const IntegerArray* src)
{
	IntegerArray* ret = IntegerArrayFromIntegers(d, src->array, src->len);
	return ret;
}

// push_front() integerArray, or prepend() integerArray
void PushFrontIntegerArray(EunInstanceData* d, IntegerArray* src, Integer digit)
{
	Integer oldLen;
	Integer* tmp;
	oldLen = src->len;
	src->len++;
	tmp = (Integer*)d->myMalloc((size_t)(src->len * sizeof(Integer)));
	if (tmp == NULL)
	{
		ErrorMessageRanOutOfMemory();
	}
	*tmp = digit;
	tmp++;
	memcpy(tmp, src->array, (size_t)(oldLen * sizeof(Integer)));
	d->myFree(src->array);
	tmp--;
	src->array = tmp;
}

void PopFrontIntegerArray(EunInstanceData* d, IntegerArray* src, size_t count)
{
	if (count)
	{
		size_t size;
		Integer* tmp;
		src->len -= count;
		if (src->len > 0)
		{
			size = (size_t)(src->len * sizeof(Integer));
			tmp = (Integer*)d->myMalloc(size);
			if (tmp == NULL)
			{
				ErrorMessageRanOutOfMemory();
			}
			memcpy(tmp, src->array + count, size);
			d->myFree(src->array);
			src->array = tmp;
		}
		else
		{
			src->len = 0;
			d->myFree(src->array);
			src->array = (Integer*)d->myMalloc(0);
		}
	}
}

void PopBackIntegerArray(EunInstanceData* d, IntegerArray* src, size_t count)
{
	if (count)
	{
		size_t size;
		Integer* tmp;
		src->len -= count;
		if (src->len > 0)
		{
			size = (size_t)(src->len * sizeof(Integer));
			tmp = (Integer*)d->myMalloc(size);
			if (tmp == NULL)
			{
				ErrorMessageRanOutOfMemory();
			}
			memcpy(tmp, src->array, size);
			d->myFree(src->array);
			src->array = tmp;
		}
		else
		{
			src->len = 0;
			d->myFree(src->array);
			src->array = (Integer*)d->myMalloc(0);
		}
	}
}

// resize() by truncating or adding zeros to the end of the integerArray
void ResizeIntegerArray(EunInstanceData* d, IntegerArray* src, Integer len)
{
	Integer* tmp;
	Integer len1;
	if (len < 0)
	{
		ErrorMessageNumberOutOfRange();
	}
	tmp = (Integer*)d->myMalloc((size_t)(len * sizeof(Integer)));
	if (tmp == NULL)
	{
		ErrorMessageRanOutOfMemory();
	}
	src->len = len; // make sure to assign len to src->len
	len1 = min(src->len, len);
	memcpy(tmp, src->array, (size_t)(len1 * sizeof(Integer)));
	d->myFree(src->array);
	src->array = tmp;
	tmp += len1; // will advance by sizeof(Integer).
	memset(tmp, 0, (size_t)((len - len1) * sizeof(Integer))); // needed, or use calloc() above instead of malloc()
}

// free integerArray
void FreeIntegerArray(EunInstanceData* d, IntegerArray* a)
{
	if (a != NULL)
	{
		d->myFree(a->array);
		d->myFree(a);
	}
}

// new Eun
Eun* NewEuNumber(EunInstanceData* d, IntegerArray* n1, Integer exp1, Integer targetLength, Integer radix, unsigned int flags)
{
	Eun* ptr;
	if ((targetLength < 2) || (radix < 2) || (radix > EUN_MAX_RADIX))
	{
		ErrorMessageNumberOutOfRange();
	}
	ptr = (Eun*)d->myMalloc(sizeof(Eun));
	if (ptr == NULL)
	{
		ErrorMessageRanOutOfMemory();
	}
	ptr->data = d;
	ptr->num = n1;
	ptr->exp = exp1;
	ptr->targetLength = targetLength;
	ptr->radix = radix;
	ptr->flags = flags;
	return ptr;
}

Eun* CopyEun(const Eun* a)
{
	IntegerArray* n1 = CopyIntegerArray(a->data, a->num);
	return NewEuNumber(a->data, n1, a->exp, a->targetLength, a->radix, a->flags);
}

// free Eun
void FreeEuNumber(Eun* a)
{
	if (a != NULL)
	{
		EunInstanceData* d = a->data;
		FreeIntegerArray(d, a->num);
		d->myFree(a);
	}
}

Float RoundTowardsZero(Float x)
{
#ifdef FLOAT80
	return (x < 0) ? ceill(x) : floorl(x);
#else
#ifdef FLOAT64
	return (x < 0) ? ceil(x) : floor(x);
#else
	return (x < 0) ? ceilf(x) : floorf(x);
#endif
#endif
}

// Boolean functions use: true=1 and false=0

Integer ArraysEqual(const IntegerArray* a, const IntegerArray* b)
{
	if (a == b)
	{
		return 1;
	}
	if (a != NULL)
	{
		if (b != NULL)
		{
			if (a->len == b->len)
			{
				Integer* x, * y, i, len;
				len = a->len;
				len--;
				x = a->array + len;
				y = b->array + len;
				for (i = len; i >= 0; i--)
				{
					if (*x-- != *y--)
					{
						return 0;
					}
				}
				return 1;
			}
		}
	}
	return 0;
}

Integer RangeArraysEqual(const IntegerArray* a, const IntegerArray* b, Integer start, Integer stop)
{
	if (a == b)
	{
		return 1;
	}
	if (a != NULL)
	{
		if (b != NULL)
		{
			if (a->len > stop)
			{
				if (b->len > stop)
				{
					Integer* x, * y, i;
					x = a->array + stop;
					y = b->array + stop;
					for (i = stop; i >= start; i--)
					{
						if (*x-- != *y--)
						{
							return 0;
						}
					}
					return 1;
				}
			}
		}
	}
	return 0;
}

void Borrow(IntegerArray* num, Integer radix)
{
	Integer* x, i;
	i = num->len;
	i--;
	x = num->array + i;
	for (; i >= 1; i--) // "i" is already initialized
	{
		if (*x >= 0)
		{
			x--;
		}
		else
		{
			*x += radix;
			x--;
			(*x)--;
		}
	}
}

void NegativeBorrow(IntegerArray* num, Integer radix)
{
	Integer* x, i;
	i = num->len;
	i--;
	x = num->array + i;
	for (; i >= 1; i--) // "i" is already initialized
	{
		if (*x >= 0)
		{
			x--;
		}
		else
		{
			*x -= radix;
			x--;
			(*x)++;
		}
	}
}

void Carry(EunInstanceData* d, IntegerArray* num, Integer radix)
{
	Integer* x, b, i;
#ifdef BITS64
	lldiv_t result;
#else
	div_t result;
#endif
	i = num->len;
	x = num->array + i;
	while (i > 0)
	{
		i--;
		x--;
		b = *x;
		if (b >= radix)
		{
#ifdef BITS64
			result = lldiv(b, radix); // division and remainder
#else
			result = div(b, radix); // division and remainder
#endif
			* x = result.rem;
			if (i == 0)
			{
				PushFrontIntegerArray(d, num, result.quot); // prepend(sum, q)
				x = num->array;
				x++;
				i++;
			}
			else
			{
				x--;
				b = *x + result.quot;
				if (b < 0) // test for integer overflow
				{
					ErrorMessageNumberOverflow();
				}
				*x = b;
				x++;
			}
		}
	}
}

void NegativeCarry(EunInstanceData* d, IntegerArray* num, Integer radix)
{
	Integer* x, b, i, negativeRadix = -radix;
#ifdef BITS64
	lldiv_t result;
#else
	div_t result;
#endif
	i = num->len;
	x = num->array + i;
	while (i > 0)
	{
		i--;
		x--;
		b = *x;
		if (b <= negativeRadix)
		{
#ifdef BITS64
			result = lldiv(b, radix); // division and remainder
#else
			result = div(b, radix); // division and remainder
#endif
			* x = result.rem;
			if (i == 0)
			{
				PushFrontIntegerArray(d, num, result.quot); // prepend(sum, q)
				x = num->array;
				x++;
				i++;
			}
			else
			{
				x--;
				b = *x + result.quot;
				if (b > 0) // test for integer overflow
				{
					ErrorMessageNumberOverflow();
				}
				*x = b;
				x++;
			}
		}
	}
}



void CarryRadixOnly(EunInstanceData* d, IntegerArray* num, Integer radix)
{
	Integer* x, b, i;
	i = num->len;
	i--;
	x = num->array + i;
	while (i >= 0)
	{
		b = *x;
		if (b != radix)
		{
			break;
		}
		*x = 0; // modulus: remainder(b, radix)
		if (i == 0)
		{
			PushFrontIntegerArray(d, num, 1); // prepend(sum, q)
			break;
		}
		else
		{
			i--;
			x--;
			(*x)++;
		}
	}
}

void NegativeCarryRadixOnly(EunInstanceData* d, IntegerArray* num, Integer radix)
{
	Integer* x, b, i, negativeRadix = -radix;;
	i = num->len;
	i--;
	x = num->array + i;
	while (i >= 0)
	{
		b = *x;
		if (b != negativeRadix)
		{
			break;
		}
		*x = 0; // modulus: remainder(b, radix)
		if (i == 0)
		{
			PushFrontIntegerArray(d, num, -1); // prepend(sum, q)
			break;
		}
		else
		{
			i--;
			x--;
			(*x)++;
		}
	}
}

//void AddTo(EunInstanceData* d, IntegerArray** addTo, IntegerArray** addFrom)
//{
//	IntegerArray* n1 = *addTo;
//	IntegerArray* n2 = *addFrom;
//	Integer* x, * y, i, c, len;
//	if (n1->len < n2->len)
//	{
//		IntegerArray* tmp = n1;
//		n1 = n2;
//		n2 = tmp;
//		// after swap, you must assign return variables:
//		*addTo = n1;
//		*addFrom = n2;
//	}
//	len = n2->len;
//	c = n1->len - len;
//	x = n1->array + c;
//	y = n2->array;
//	for (i = 0; i < len; i++)
//	{
//		(*x) += (*y);
//		x++;
//		y++;
//	}
//}

IntegerArray* Add(EunInstanceData* d, const IntegerArray* n1, const IntegerArray* n2, Integer isSubtraction)
{
	IntegerArray* numArray;
	Integer* x, * y, i, len;
	if (n1->len >= n2->len)
	{
		// copy n1 to sum:
		numArray = CopyIntegerArray(d, n1);
		len = n2->len;
		x = numArray->array + (n1->len - len);
		y = n2->array;
	}
	else
	{
		// copy n1 to sum:
		numArray = CopyIntegerArray(d, n2);
		len = n1->len;
		x = numArray->array + (n2->len - len);
		y = n1->array;
	}
	if (isSubtraction)
	{
		for (i = 0; i < len; i++)
		{
			(*x) -= (*y);
			x++;
			y++;
		}
	}
	else
	{
		for (i = 0; i < len; i++)
		{
			(*x) += (*y);
			x++;
			y++;
		}
	}
	return numArray;
}

IntegerArray* ConvertRadix(EunInstanceData* d, const IntegerArray* number, Integer fromRadix, Integer toRadix)
{
	IntegerArray* target, * base, * tmp, * tmp1;
	Integer i, j, len, digit;
	Integer* x;
	void(*mycarry)(EunInstanceData*, IntegerArray*, Integer);
	target = NewIntegerArray(d, 0);
	if (number->len > 0)
	{
		base = NewIntegerArray(d, 1);
		*base->array = 1;
		mycarry = (*number->array < 0) ? NegativeCarry : Carry;
		for (i = number->len - 1; i >= 0; i--)
		{
			tmp = CopyIntegerArray(d, base);
			digit = number->array[i];
			len = tmp->len;
			x = tmp->array;
			for (j = 0; j < len; j++)
			{
				(*x) *= digit;
				x++;
			}
			tmp1 = target;
			target = Add(d, tmp1, tmp, 0);
			FreeIntegerArray(d, tmp);
			FreeIntegerArray(d, tmp1);
			(*mycarry)(d, target, toRadix);
			len = base->len;
			x = base->array;
			for (j = 0; j < len; j++)
			{
				(*x) *= fromRadix;
				x++;
			}
			Carry(d, base, toRadix);
		}
		FreeIntegerArray(d, base);
	}
	return target;
}

IntegerArray* Mult(EunInstanceData* d, const IntegerArray* n1, const IntegerArray* n2)
{
	IntegerArray* numArray;
	Integer len1 = n1->len;
	Integer len2 = n2->len;
	if ((len1 == 0) || (len2 == 0))
	{
		numArray = NewIntegerArray(d, 0);
		return numArray;
	}
	Integer myLen = len1 + len2;
	myLen--;
	numArray = NewIntegerArray(d, myLen);
	Integer* resultArrayPtr, * arg1, * p2, * arg2, f, g, * h, i, j, k;
	resultArrayPtr = numArray->array;
	f = (*n1->array < 0) != (*n2->array < 0);
	arg1 = n1->array;
	p2 = n2->array;
	for (i = 0; i < len1; i++)
	{
		h = resultArrayPtr;
		g = *arg1;
		arg2 = p2;
		for (j = 0; j < len2; j++)
		{
			k = g * (*arg2) + (*h);
			if (f)
			{
				if (k > 0) // test for integer overflow
				{
					ErrorMessageNumberOverflow();
				}
			}
			else
			{
			// make result positive, then test for errors on negative
				if (k < 0) // test for integer overflow
				{
					ErrorMessageNumberOverflow();
				}
			}
			(*h) = k;
			h++;
			arg2++;
		}
		arg1++;
		resultArrayPtr++;
	}
	return numArray;
}

int IsNegative(const IntegerArray* a)
{
	if (a->len > 0)
	{
		return (*a->array < 0);
	}
	return 0;
}

void Negate(IntegerArray* toNegate)
{
	Integer i, len;
	Integer* x = toNegate->array;
	len = toNegate->len;
	for (i = 0; i < len; i++)
	{
		*x *= -1;
		x++;
	}
}

void AbsoluteValue(IntegerArray* toNegate)
{
	if (toNegate->len > 0)
	{
		if (*toNegate->array < 0)
		{
			Negate(toNegate);
		}
	}
}

void Subtract(EunInstanceData* d, IntegerArray* num, Integer radix, Integer isMixed)
{
	if (num->len > 0)
	{
		if (*num->array < 0)
		{
			NegativeCarry(d, num, radix);
			if (isMixed)
			{
				NegativeBorrow(num, radix);
			}
		}
		else
		{
			Carry(d, num, radix);
			if (isMixed)
			{
				Borrow(num, radix);
			}
		}
	}
}

void TrimLeadingZeros(EunInstanceData* d, IntegerArray* num)
{
	Integer i, len = num->len;
	Integer* ptr = num->array;
	for (i = 0; i < len; i++)
	{
		if (*ptr++ != 0)
		{
			break;
		}
	}
	PopFrontIntegerArray(d, num, (size_t)i);
}

void TrimTrailingZeros(EunInstanceData* d, IntegerArray* num)
{
	Integer i, len = num->len;
	Integer* ptr = num->array + len;
	ptr--;
	for (i = 0; i < len; i++)
	{
		if (*ptr-- != 0)
		{
			break;
		}
	}
	PopBackIntegerArray(d, num, (size_t)i);
}

void AdjustRound(Eun* a, Integer isMixed)
{
	Integer round2, oldlen, halfRadix, f, i, roundTargetLength;
	EunInstanceData* d;
	oldlen = a->num->len;
	d = a->data;
	TrimLeadingZeros(d, a->num);
	a->exp += (a->num->len - oldlen);
	// adjust_exponent();
	oldlen = a->num->len;
	// in Subtract, the first element of sum cannot be a zero.
	Subtract(d, a->num, a->radix, isMixed);
	// use Subtract() when there are both negative and positive numbers.
	// otherwise, you can use Carry(). (for all positive numbers)
	// sum = Carry(sum, radix);
	TrimLeadingZeros(d, a->num);
	a->exp += (a->num->len - oldlen);
	if (a->num->len <= 0)
	{
		return;
	}
	//roundingMethod();
	halfRadix = a->radix >> 1; // floor(radix / 2)
	if (d->roundToNearestOption != 0)
	{
		roundTargetLength = a->exp + d->adjustRound;
		roundTargetLength++;
		if (a->targetLength < roundTargetLength)
		{
			a->targetLength = roundTargetLength;
		}
		if (roundTargetLength <= 1)
		{
			if (a->exp <= -1)
			{
				if (a->exp == -1)
				{
					PushFrontIntegerArray(d, a->num, 0);
				}
				else
				{
					FreeIntegerArray(d, a->num);
					a->num = NewIntegerArray(d, 2);
				}
				a->exp = 0; // zero because it rounds to nearest integer
			}
			roundTargetLength = 1;
		}
	}
	else
	{
		roundTargetLength = a->targetLength - d->adjustRound;
		if (roundTargetLength <= 0)
		{
			if (roundTargetLength == 0)
			{
				PushFrontIntegerArray(d, a->num, 0);
				a->exp++;
			}
			else
			{
				FreeIntegerArray(d, a->num);
				a->num = NewIntegerArray(d, 2);
			}
			roundTargetLength = 1;
		}
	}
	oldlen = a->num->len;
	if (oldlen > roundTargetLength)
	{
		f = a->num->array[roundTargetLength];
		if ((a->radix & 1) == 1) // if (IsIntegerOdd(radix))
		{
			// feature: support for odd radixes
			// This seems to work:
			for (i = roundTargetLength + 1; i < oldlen; i++)
			{
				if ((f != halfRadix) && (f != -halfRadix))
				{
					break;
				}
				f = a->num->array[i];
			}
		}
		round2 = d->roundingMethod;
		if ((f == halfRadix) || (f == -halfRadix))
		{
			switch (round2)
			{
			case ROUND_EVEN:
				halfRadix -= (a->num->array[roundTargetLength - 1] & 1); // IsIntegerOdd
				break;
			case ROUND_ODD:
				halfRadix -= (a->num->array[roundTargetLength - 1] & 0); // IsIntegerEven
				break;
			case ROUND_ZERO:
				f = 0;
				break;
			default:
				break;
			}
		}
		else
		{
			switch (round2)
			{
			case ROUND_INF:
				halfRadix--;
				break;
			case ROUND_POS_INF:
				f++;
				break;
			case ROUND_NEG_INF:
				f--;
				break;
			default:
				break;
			}
		}
		ResizeIntegerArray(d, a->num, roundTargetLength);
		if (halfRadix < f)
		{
			a->num->array[roundTargetLength - 1]++;
			CarryRadixOnly(d, a->num, a->radix);
			a->exp += (a->num->len - roundTargetLength);
		}
		else if (f < -halfRadix)
		{
			a->num->array[roundTargetLength - 1]--;
			NegativeCarryRadixOnly(d, a->num, a->radix);
			a->exp += (a->num->len - roundTargetLength);
		}
	}
	TrimTrailingZeros(d, a->num);
	oldlen = a->num->len;
	TrimLeadingZeros(d, a->num);
	a->exp += (a->num->len - oldlen);
}

Eun* MultExp(EunInstanceData* d, const IntegerArray* n1, Integer exp1, const IntegerArray* n2, Integer exp2, Integer targetLength, Integer radix)
{
	Eun* ret;
	IntegerArray* numArray;
	numArray = Mult(d, n1, n2);
	// numArray = Mult(d, n1, n2, radix); // uncomment to use almost any radix you want, comment out above line
	ret = NewEuNumber(d, numArray, exp1 + exp2, targetLength, radix, 0);
	AdjustRound(ret, TRUE); // could be "FALSE", but arrays could be mixed so it should be "TRUE"
	return ret;
}

Eun* AddExp(EunInstanceData* d, const IntegerArray* n1, Integer exp1, const IntegerArray* n2, Integer exp2, Integer targetLength, Integer radix, Integer isSubtraction)
{
	Eun* ret;
	IntegerArray* local, * n0;
	const IntegerArray* p1;
	const IntegerArray* p2;
	Integer size = (n1->len - exp1) - (n2->len - exp2);
	if (size < 0)
	{
		size = n1->len - size;
		local = NewIntegerArray(d, size);
		memcpy(local->array, n1->array, (size_t)(n1->len * sizeof(Integer)));
		p1 = local;
		p2 = n2;
	}
	else if (0 < size)
	{
		size += n2->len;
		local = NewIntegerArray(d, size);
		memcpy(local->array, n2->array, (size_t)(n2->len * sizeof(Integer)));
		p1 = n1;
		p2 = local;
	}
	else
	{
		p1 = n1;
		p2 = n2;
		local = NULL;
	}
	if (exp1 < exp2)
	{
		exp1 = exp2;
	}
	n0 = Add(d, p1, p2, isSubtraction);
	FreeIntegerArray(d, local);
	ret = NewEuNumber(d, n0, exp1, targetLength, radix, 0);
	AdjustRound(ret, TRUE);
	return ret;
}

Eun* SubtractExp(EunInstanceData* d, const IntegerArray* n1, Integer exp1, const IntegerArray* n2, Integer exp2, Integer targetLength, Integer radix)
{
	return AddExp(d, n1, exp1, n2, exp2, targetLength, radix, TRUE);
}


// Division and Mult Inverse:
// https://en.wikipedia.org/wiki/Newton%27s_method#Multiplicative_inverses_of_numbers_and_power_series

Eun* ProtoMultInvExp(EunInstanceData* d, const IntegerArray* n0, Integer exp0, const IntegerArray* n1, Integer exp1, Integer targetLength, Integer radix)
{
	// a = in0
	// n1 = in1
	// f(a) = a * (2 - (n1 * a))
	Eun* tmp1, * tmp;
	tmp = MultExp(d, n0, exp0, n1, exp1, targetLength, radix); // n1 * a
	tmp1 = SubtractExp(d, &valTwo, 0, tmp->num, tmp->exp, targetLength, radix); // 2 - tmp
	FreeEuNumber(tmp);
	tmp = MultExp(d, n0, exp0, tmp1->num, tmp1->exp, targetLength, radix); // a * tmp
	FreeEuNumber(tmp1);
	return tmp;
}

IntegerArray* IntToDigits(EunInstanceData* d, Float x, Integer radix)
{
	IntegerArray* numArray = NewIntegerArray(d, 0);
	Float a, r = (Float)radix;
	while (x != 0)
	{
#ifdef FLOAT80
		a = fmodl(x, r); // remainder(p, radix)
#else
#ifdef FLOAT64
		a = fmod(x, r); // remainder(p, radix)
#else
		a = fmodf(x, r); // remainder(p, radix)
#endif
#endif
		PushFrontIntegerArray(d, numArray, (Integer)a);
		x = RoundTowardsZero(x / r); // integer division, floor(p / radix), round() works on negative numbers
	}
	return numArray;
}

ExpToAtomRet* ExpToAtom(EunInstanceData* d, const IntegerArray* n1, Integer exp1, Integer targetLength, Integer radix)
{
	Float p, ans, lookat;
	Integer overFlowBy, len, i, ele;
	ExpToAtomRet* ret;
	if (n1->len <= 0)
	{
		return NULL; // tried to divide by zero.
	}
	// what if exp1 is too large? (which is a bad thing in this case)
	// Make it smaller.
#ifdef FLOAT80
	p = logl((Float)radix);
#else
#ifdef FLOAT64
	p = log((Float)radix);
#else
	p = logf((Float)radix);
#endif
#endif
	overFlowBy = exp1 - (Integer)(logMax / p) + 2; // floor(), +2 may need to be increased
	if (overFlowBy > 0)
	{
		// overflow warning in "power()" function
		// reduce size
		exp1 -= overFlowBy;
	}
	else
	{
		// what if exp1 is too small?
		overFlowBy = exp1 - (Integer)(logMin / p) - 2; // floor(), -2 may need to be decreased
		if (overFlowBy < 0)
		{
			exp1 -= overFlowBy;
		}
		else
		{
			overFlowBy = 0;
		}
	}
	exp1 -= targetLength;
	len = n1->len;
	// NOTE: I don't think I need this section (1a):
	//Begin Section (1a):
	// if (targetLength < len)
	// {
	//		len = targetLength;
	// }
	// if (len < 0)
	// {
	//		exp1 += len;
	//		len = n1->len;
	// }
	//End Section (1a).
#ifdef FLOAT80
	p = powl((Float)radix, (Float)exp1); // integer to Float, correct
#else
#ifdef FLOAT64
	p = pow((Float)radix, (Float)exp1); // integer to Float, correct
#else
	p = powf((Float)radix, (Float)exp1); // integer to Float, correct
#endif
#endif
	ans = (*n1->array) * p;
	for (i = 1; i < len; i++)
	{
		p /= radix; // "p = (p / radix);"
		ele = n1->array[i];
		if (ele != 0)
		{
			lookat = ans;
			ans += ele * p;
			if (ans == lookat)
			{
				break;
			}
		}
	}
	// if overFlowBy is positive, then there was an overflow
	// if overFlowBy is negative, then there was an underflow
	// overFlowBy is an offset of that overflow in the given radix
	ret = (ExpToAtomRet*)d->myMalloc(sizeof(ExpToAtomRet));
	if (ret == NULL)
	{
		ErrorMessageRanOutOfMemory();
	}
	ret->a = ans;
	ret->i = overFlowBy;
	return ret;
}

Eun* GetGuessExp(EunInstanceData* d, const IntegerArray* den, Integer exp1, Integer protoTargetLength, Integer radix)
{
	Eun* retEun;
	IntegerArray* guess;
	ExpToAtomRet* tmp;
	Float denom, one, ans;
	Integer overflowBy, len, sigDigits;
#ifdef FLOAT80
	sigDigits = (Integer)ceill(18 / log10l((long double)radix));
#else
#ifdef FLOAT64
	sigDigits = (Integer)ceil(15 / log10((double)radix));
#else
	sigDigits = (Integer)ceilf(6 / log10f((float)radix));
#endif
#endif
	if (protoTargetLength < sigDigits)
	{
		sigDigits = protoTargetLength;
	}
	len = den->len;
	tmp = ExpToAtom(d, den, len - 1, sigDigits, radix);
	denom = tmp->a;
	overflowBy = tmp->i;
	d->myFree(tmp);
#ifdef FLOAT80
	one = powl((Float)radix, (Float)(len - 1 - overflowBy));
#else
#ifdef FLOAT64
	one = pow((Float)radix, (Float)(len - 1 - overflowBy));
#else
	one = powf((Float)radix, (Float)(len - 1 - overflowBy));
#endif
#endif
	ans = RoundTowardsZero(one / denom);
	guess = IntToDigits(d, ans, radix); // works on negative numbers
	retEun = NewEuNumber(d, guess, exp1, sigDigits - 1, radix, 0);
	AdjustRound(retEun, FALSE);
	retEun->targetLength = protoTargetLength;
	return retEun;
}

Eun* MultInvExp(EunInstanceData* d, const IntegerArray* den1, Integer exp1, Integer targetLength, Integer radix)
{
	Eun* tmp, * lookat;
	IntegerArray* guess;
	Integer i, exp0, protoTargetLength, len, lastLen, moreAccuracy;
	if (den1->len <= 0)
	{
		d->lastIterCount = 1;
		tmp = NewEuNumber(d, NewIntegerArray(d, 0), 0, targetLength, radix, 0);
		tmp->divideByZeroFlag = 1;
		tmp->infinityFlag = 1;
		d->myErrorCallBack(1);
		return tmp;
	}
	if (den1->len == 1) // optimization
	{
		if ((*den1->array == 1) || (*den1->array == -1))
		{
			d->lastIterCount = 1;
			tmp = NewEuNumber(d, CopyIntegerArray(d, den1), (-exp1), targetLength, radix, 0);
			return tmp;
		}
	}
	if (d->multInvMoreAccuracy > 0)
	{
		moreAccuracy = d->multInvMoreAccuracy;
	}
	else
	{
#ifdef FLOAT80
		moreAccuracy = (Integer)ceill(((Float)targetLength) / d->calcSpeed);
#else
#ifdef FLOAT64
		moreAccuracy = (Integer)ceil(((Float)targetLength) / d->calcSpeed);
#else
		moreAccuracy = (Integer)ceilf(((Float)targetLength) / d->calcSpeed);
#endif
#endif
	}
	targetLength += moreAccuracy;
	protoTargetLength = targetLength + moreAccuracy;
	exp0 = (-exp1);
	exp0--;
	tmp = GetGuessExp(d, den1, exp0, protoTargetLength, radix);
	guess = tmp->num;
	lastLen = guess->len;
	d->lastIterCount = maxIter;
	for (i = 1; i <= maxIter; i++)
	{
		lookat = tmp;
		tmp = ProtoMultInvExp(d, guess, exp0, den1, exp1, protoTargetLength, radix);
		guess = tmp->num;
		exp0 = tmp->exp;
		len = guess->len;
		if (len > targetLength)
		{
			len = targetLength;
		}
		if (len == lastLen)
		{
			if (exp0 == lookat->exp)
			{
				if (RangeArraysEqual(guess, lookat->num, 0, len - 1))
				{
					//if (d->lastIterCount == i - 1) {
					//	FreeEuNumber(lookat);
					//	break;
					//}
					d->lastIterCount = i;
					break;
				}
			}
		}
		FreeEuNumber(lookat);
		lastLen = len;
	}
	targetLength -= moreAccuracy;
	tmp->targetLength = targetLength;
	AdjustRound(tmp, FALSE);
	return tmp;
}

Eun* DivideExp(EunInstanceData* d, const IntegerArray* n1, Integer exp1, const IntegerArray* n2, Integer exp2, Integer targetLength, Integer radix)
{
	Eun* tmp, * ret;
	if (n2->len <= 0) // divided by zero: (n1 / 0)
	{
		d->lastIterCount = 0;
		tmp = NewEuNumber(d, NewIntegerArray(d, 0), 0, targetLength, radix, 0);
		tmp->divideByZeroFlag = 1;
		tmp->infinityFlag = 1;
		if (n1->len <= 0)
		{
			tmp->nanFlag = 1;
		}
		else
		{
			tmp->infinityFlag += IsNegative(n1);
		}
		d->myErrorCallBack(1);
		return tmp;
	}
	tmp = MultInvExp(d, n2, exp2, targetLength, radix);
	if (tmp->divideByZeroFlag == 0)
	{
		ret = MultExp(d, n1, exp1, tmp->num, tmp->exp, targetLength, radix);
		FreeEuNumber(tmp);
		return ret;
	}
	else // divided by zero: (n1 / 0)
	{
		if (n1->len <= 0)
		{
			tmp->nanFlag = 1;
		}
		else
		{
			tmp->infinityFlag += IsNegative(n1);
		}
		return tmp;
	}
}


Eun* ConvertExp(EunInstanceData* d, const IntegerArray* n1, Integer exp1, Integer targetLength, Integer fromRadix, Integer toRadix)
{
	IntegerArray* n2, * n3, * local, * p1;
	Eun* result;
	Integer exp2, exp3, len;
	TrimTrailingZeros(d, n1);
	if (n1->len <= 0)
	{
		result = NewEuNumber(d, NewIntegerArray(d, 0), 0, targetLength, toRadix, 0);
		return result;
	}
	if (n1->len <= exp1)
	{
		local = NewIntegerArray(d, exp1 + 1);
		memcpy(local->array, n1->array, (size_t)(n1->len * sizeof(Integer)));
		p1 = local; // remember to free.
	}
	else
	{
		local = NULL;
		p1 = (IntegerArray*)n1;
	}
	n2 = ConvertRadix(d, p1, fromRadix, toRadix);
	exp2 = n2->len;
	exp2--;
	len = p1->len;
	FreeIntegerArray(d, local); // local freed.
	local = NewIntegerArray(d, len - exp1);
	*local->array = 1;
	n3 = ConvertRadix(d, local, fromRadix, toRadix);
	FreeIntegerArray(d, local); // local freed.
	exp3 = n3->len;
	exp3--;
	result = DivideExp(d, n2, exp2, n3, exp3, targetLength, toRadix);
	FreeIntegerArray(d, n2);
	FreeIntegerArray(d, n3);
	return result;
}


// NOTE: Begin "Eun" functions:

static char indent[] = "  ";

void EunPrint(const Eun* n1)
{
	Integer i, len;
	char buf[80];
	Integer charsPrinted, sum, sizeofIndent;
	len = n1->num->len;
	sizeofIndent = strlen(indent);
	printf("{\n%s{", indent);
	if (len != 0) {
		Integer* a;
		sum = sizeofIndent;
		sum++;
		len--;
		a = n1->num->array;
		for (i = 0; i < len; i++) {
#ifdef BITS64
			charsPrinted = snprintf((char* const)&buf, 80, "%lli,", *a++);
#else
			charsPrinted = snprintf((char *const)&buf, 80, "%i,", *a++);
#endif
			sum += charsPrinted;
			if (sum >= 78) // line-wrap at column 78
			{
				printf("\n");
				sum = 0;
			}
			if (charsPrinted > 0)
			{
				printf("%s", buf);
			}
		}
#ifdef BITS64
		charsPrinted = snprintf((char* const)& buf, 80, "%lli", *a);
#else
		charsPrinted = snprintf((char* const)& buf, 80, "%i", *a);
#endif
		sum += charsPrinted + 2;
		if (sum >= 78) // line-wrap at column 78
		{
			printf("\n");
			sum = 0;
		}
		if (charsPrinted > 0)
		{
			printf("%s", buf);
		}
	}
#ifdef BITS64
	printf("},\n%s%lli,\n%s%lli,\n%s%lli\n}\n", indent, n1->exp, indent, n1->targetLength, indent, n1->radix);
#else
	printf("},\n%s%i,\n%s%i,\n%s%i\n}\n", indent, n1->exp, indent, n1->targetLength, indent, n1->radix);
#endif
}

void EunAdjustRound(Eun* n1, Integer adjustBy)
{
	if (adjustBy >= 0)
	{
		Integer tmp;
		tmp = n1->data->adjustRound;
		n1->data->adjustRound = adjustBy;
		AdjustRound(n1, TRUE);
		n1->data->adjustRound = tmp;
	}
	else
	{
		AdjustRound(n1, TRUE);
	}
}

Eun* EunMult(const Eun* n1, const Eun* n2)
{
	Integer targetLength;
	if (n1->data != n2->data)
	{
		return NULL;
	}
	if (n1->radix != n2->radix)
	{
		return NULL;
	}
	if (n1->targetLength >= n2->targetLength)
	{
		targetLength = n1->targetLength;
	}
	else
	{
		targetLength = n2->targetLength;
	}
	return MultExp(n1->data, n1->num, n1->exp, n2->num, n2->exp, targetLength, n1->radix);
}

Eun* EunAdd(const Eun* n1, const Eun* n2)
{
	Integer targetLength;
	if (n1->data != n2->data)
	{
		return NULL;
	}
	if (n1->radix != n2->radix)
	{
		return NULL;
	}
	if (n1->targetLength >= n2->targetLength)
	{
		targetLength = n1->targetLength;
	}
	else
	{
		targetLength = n2->targetLength;
	}
	return AddExp(n1->data, n1->num, n1->exp, n2->num, n2->exp, targetLength, n1->radix, FALSE);
}

int EunIsNegative(const Eun* n1)
{
	return IsNegative(n1->num);
}

Eun* EunNegate(const Eun* n1)
{
	Eun* ret = CopyEun(n1);
	Negate(ret->num);
	return ret;
}

Eun* EunSubtract(const Eun* n1, const Eun* n2)
{
	Integer targetLength;
	if (n1->data != n2->data)
	{
		return NULL;
	}
	if (n1->radix != n2->radix)
	{
		return NULL;
	}
	if (n1->targetLength >= n2->targetLength)
	{
		targetLength = n1->targetLength;
	}
	else
	{
		targetLength = n2->targetLength;
	}
	return SubtractExp(n1->data, n1->num, n1->exp, n2->num, n2->exp, targetLength, n1->radix);
}

Eun* EunMultInv(const Eun* n1)
{
	return MultInvExp(n1->data, n1->num, n1->exp, n1->targetLength, n1->radix);
}

Eun* EunDivide(const Eun* n1, const Eun* n2)
{
	Integer targetLength;
	if (n1->data != n2->data)
	{
		return NULL;
	}
	if (n1->radix != n2->radix)
	{
		return NULL;
	}
	if (n1->targetLength >= n2->targetLength)
	{
		targetLength = n1->targetLength;
	}
	else
	{
		targetLength = n2->targetLength;
	}
	return DivideExp(n1->data, n1->num, n1->exp, n2->num, n2->exp, targetLength, n1->radix);
}

Eun* EunConvert(const Eun* n1, Integer to_radix, Integer targetLength)
{
	return ConvertExp(n1->data, n1->num, n1->exp, targetLength, n1->radix, to_radix);
}

Integer CompareExp(IntegerArray* n1, Integer exp1, IntegerArray* n2, Integer exp2)
{
	IntegerArray * a, * b;
	Integer* x, * y, flag, i, len;
	if (n1->len <= 0)
	{
		n1 = &valZero;
		exp1 = exp2;
	}
	if (n2->len <= 0)
	{
		n2 = &valZero;
		exp2 = exp1;
	}
	if (exp1 > exp2)
	{
		return 1;
	}
	else if (exp1 < exp2)
	{
		return -1;
	}
	if (n1->len >= n2->len)
	{
		a = n2;
		b = n1;
		flag = 1;
	}
	else
	{
		a = n1;
		b = n2;
		flag = -1;
	}
	len = a->len;
	x = a->array;
	y = b->array;
	for (i = 0; i < len; i++)
	{
		if (*x != *y)
		{
			if (*x > *y)
			{
				return -flag;
			}
			else
			{
				return flag;
			}
		}
		x++;
		y++;
	}
	if (a->len == b->len)
	{
		return 0;
	}
	if (*b->array < 0)
	{
		return -flag;
	}
	else
	{
		return flag;
	}
}

Integer EunCompare(const Eun* n1, const Eun* n2)
{
	if (n1->radix != n2->radix)
	{
		n1->data->myErrorCallBack(4);
	}
	return CompareExp(n1->num, n1->exp, n2->num, n2->exp);
}

IntegerArray* ReverseIntegerArray(EunInstanceData* d, const IntegerArray* s)
{
	Integer upper, lower, n, n2;
	IntegerArray* t;
	n = s->len;
	n2 = (Integer)(n / 2);
	t = NewIntegerArray(d, n);
	lower = 0;
	for (upper = n - 1; upper >= n2; upper--)
	{
		t->array[upper] = s->array[lower];
		t->array[lower] = s->array[upper];
		lower++;
	}
	return t;
}

Eun* EunReverse(const Eun* n1)
{
	IntegerArray* a;
	Eun* ret;
	a = ReverseIntegerArray(n1->data, n1->num);
	ret = NewEuNumber(n1->data, a, n1->exp, n1->targetLength, n1->radix, 0);
	return ret;
}

Eun* EunFracPart(const Eun* n1)
{
	Eun* ret;
	if (n1->exp < 0)
	{
		return CopyEun(n1);
	}
	else
	{
		if (n1->exp + 1 < n1->num->len)
		{
			// NOTE: This part works:
			IntegerArray local;
			local.array = n1->num->array;
			local.array += n1->exp;
			local.array++;
			local.len = n1->num->len - n1->exp;
			local.len--;
			ret = NewEuNumber(n1->data, CopyIntegerArray(n1->data, &local), -1, n1->targetLength, n1->radix, 0);
		}
		else
		{
			ret = NewEuNumber(n1->data, NewIntegerArray(n1->data, 0), 0, n1->targetLength, n1->radix, 0);
		}
	}
	return ret;
}

Eun* EunIntPart(const Eun* n1)
{
	Eun* ret;
	Integer len;
	len = n1->exp;
	len++;
	if (len >= n1->num->len)
	{
		return CopyEun(n1);
	}
	else
	{
		if (n1->exp >= 0)
		{
			// NOTE: This part works:
			IntegerArray local;
			local.array = n1->num->array;
			local.len = n1->exp;
			local.len++;
			ret = NewEuNumber(n1->data, CopyIntegerArray(n1->data, &local), n1->exp, n1->targetLength, n1->radix, 0);
		}
		else
		{
			ret = NewEuNumber(n1->data, NewIntegerArray(n1->data, 0), 0, n1->targetLength, n1->radix, 0);
		}
	}
	return ret;
}

void EunRoundSig(Eun* n1, Integer sigDigits)
{
	Integer tmp;
	tmp = n1->targetLength;
	n1->targetLength = sigDigits;
	AdjustRound(n1, TRUE);
	n1->targetLength = tmp;
}

void EunRoundSignificantDigits(Eun* n1, Integer sigDigits)
{
	EunRoundSig(n1, sigDigits);
}

void EunRoundToInt(Eun* n1)
{
	if (n1->exp < -1)
	{
		FreeIntegerArray(n1->data, n1->num);
		n1->num = NewIntegerArray(n1->data, 0);
	}
	else
	{
		EunRoundSig(n1, n1->exp + 1);
	}
}

Eun* EunCombInt(const Eun* in1, Integer adjustBy)
{
	// if there is any fraction part, add or subtract one, away from zero
	Eun* n1 = CopyEun(in1);
	Integer len, exponent;
	EunAdjustRound(n1, adjustBy);
	len = n1->num->len;
	if (len > 0)
	{
		Eun* tmp = n1;
		exponent = n1->exp;
		n1 = EunIntPart(tmp);
		FreeEuNumber(tmp);
		if ((exponent < 0) || (exponent + 1 < len))
		{
			// add one, same sign
			tmp = n1;
			n1 = AddExp(n1->data, n1->num, n1->exp, &valOne, 0, n1->targetLength, n1->radix, (*n1->num->array < 0));
			FreeEuNumber(tmp);
		}
	}
	return n1;
}

// input and output, type conversion functions:

char* AllocateString(EunInstanceData* d, const char* in)
{
	size_t size = strlen(in);
	size++;
	char* ret = (char*)d->myMalloc(size);
	if (ret == NULL)
	{
		ErrorMessageRanOutOfMemory();
	}
	memcpy(ret, in, size);
	return ret;
}

Eun* StringToEun(EunInstanceData* d, const char* inbuf, Integer radix, Integer targetLength)
{
	Eun* ret;
	Integer* x, isSigned, exp, len;
	IntegerArray* num;
	char* buf, * tmp, * st, ch;
	len = strlen(inbuf);
	if (len == 0)
	{
		return NULL; // undefined
	}
	st = (char*)inbuf; // get pointer to character in buf
	isSigned = (*st == '-') ? 1 : 0;
	if ((isSigned == 1) || (*st == '+'))
	{
		st++;
	}
	if (strcmp(st, "nan") == 0) // equal(st, "nan")
	{
		ret = NewEuNumber(d, NewIntegerArray(d, 0), 0, d->defaultTargetLength, d->defaultRadix, 0);
		ret->nanFlag = 1;
		ret->infinityFlag = (isSigned == 0) ? MY_POS_INFINITY : MY_NEG_INFINITY;
		return ret;
	}
	if (strcmp(st, "inf") == 0) // equal(st, "inf")
	{
		ret = NewEuNumber(d, NewIntegerArray(d, 0), 0, d->defaultTargetLength, d->defaultRadix, 0);
		ret->infinityFlag = (isSigned == 0) ? MY_POS_INFINITY : MY_NEG_INFINITY;
		return ret;
	}
	// first time:
	buf = AllocateString(d, st);
	st = buf;
	tmp = strchr(buf, 'e');
	if (tmp == NULL)
	{
		tmp = strchr(buf, 'E');
	}
	if (tmp != NULL)
	{
#ifdef BITS64
		exp = strtoll(tmp + 1, NULL, 10);
#else
		exp = strtol(tmp + 1, NULL, 10);
#endif
		* tmp = '\0';
	}
	else
	{
		exp = 0;
		tmp = strchr(buf, 0); // seek to the end of buf
	}
	while ((st < tmp) && (*st == '0'))
	{
		st++;
	}
	// second time:
	tmp = strchr(st, '.');
	if (tmp != NULL)
	{
		len = strlen(tmp + 1);
		memmove(tmp, tmp + 1, (size_t)len);
		tmp[len] = '\0';
		exp += (tmp - st); // ok.
	}
	else
	{
		exp += strlen(st);
	}
	exp--;
	while ((st < tmp) && (*st == '0'))
	{
		exp--;
		st++;
	}
	len = strlen(st);
	st += len;
	st--;
	while ((len > 0) && (*st == '0'))
	{
		st--;
		len--;
	}
	if (len == 0)
	{
		exp = 0;
	}
	num = NewIntegerArray(d, len);
	x = num->array + len;
	x--;
	while (len > 0)
	{
		ch = *st;
		if ((ch > '9') || (ch < '0'))
		{
			d->myFree(buf);
			ret = NewEuNumber(d, NewIntegerArray(d, 0), 0, d->defaultTargetLength, d->defaultRadix, 0);
			ret->nanFlag = 1;
			d->myErrorCallBack(3);
			return ret;
		}
		*x = (Integer)(ch - '0');
		x--;
		st--;
		len--;
	}
	d->myFree(buf);
	if (isSigned == 1)
	{
		Negate(num);
	}
	if (radix == 0)
	{
		radix = d->defaultRadix;
	}
	if (targetLength == 0)
	{
		targetLength = d->defaultTargetLength;
	}
#ifdef FLOAT80
	ret = NewEuNumber(d, num, exp, (Integer)ceill(log10l((Float)radix) * (Float)targetLength), 10, 0);
#else
#ifdef FLOAT64
	ret = NewEuNumber(d, num, exp, (Integer)ceil(log10((Float)radix) * (Float)targetLength), 10, 0);
#else
	ret = NewEuNumber(d, num, exp, (Integer)ceilf(log10f((Float)radix) * (Float)targetLength), 10, 0);
#endif
#endif
	if (radix != 10)
	{
		Eun* euntmp = ret;
		ret = EunConvert(euntmp, radix, targetLength);
		FreeEuNumber(euntmp);
	}
	return ret;
}

Eun* ToEun(EunInstanceData* d, Float a, Integer radix, Integer targetLength)
{
	char buf[80];
#ifdef FLOAT80
	sprintf_s(buf, 80, "%.17e", a);
#else
#ifdef FLOAT64
	sprintf_s(buf, 80, "%.14e", a);
#else
	sprintf_s(buf, 80, "%.5e", a);
#endif
#endif
	return StringToEun(d, buf, radix, targetLength);
}

char* ToString(const Eun* n1)
{
	Eun* local;
	char* st, * tmp, * ret, buf[80];
	IntegerArray* a;
	Integer* x, len, i, f, exp1;
	size_t size;
	if (n1->infinityFlag > 0)
	{
		if (n1->nanFlag == 1)
		{
			return AllocateString(n1->data, "-nan");
		}
		if (n1->infinityFlag == MY_POS_INFINITY)
		{
			return AllocateString(n1->data, "inf");
		}
		if (n1->infinityFlag == MY_NEG_INFINITY)
		{
			return AllocateString(n1->data, "-inf");
		}
	}
	if (n1->radix != 10)
	{
		local = EunConvert(n1, 10, (Integer)ceil(log10((double)n1->radix) * (double)n1->targetLength));
		a = local->num;
		exp1 = local->exp;
	}
	else
	{
		local = NULL;
		a = n1->num;
		exp1 = n1->exp;
	}
	if (n1->num->len <= 0)
	{
		return AllocateString(n1->data, "0");
	}
	len = a->len;
	f = (*a->array < 0) ? 1 : 0;
	i = (len > 1) ? 1 : 0;
	tmp = (char*)n1->data->myMalloc((size_t)(len + f + i + 1));
	if (tmp == NULL)
	{
		ErrorMessageRanOutOfMemory();
	}
	st = tmp;
	if (f) // a->array[0] < 0
	{
		*st = '-'; // st[0]
		st++;
		*st = (char)(48 - *a->array); // st[0], a->array[0]
	}
	else
	{
		*st = (char)(*a->array + 48); // st[0], a->array[0]
	}
	if (i)
	{
		st++;
		*st = '.';
		st++;
		x = a->array;
		x++;
		if (f == 1)
		{
			for (i = 1; i < len; i++)
			{
				*st = (char)(48 - *x);
				st++;
				x++;
			}
		}
		else
		{
			for (i = 1; i < len; i++)
			{
				*st = (char)(*x + 48);
				st++;
				x++;
			}
		}
		f++; // this line is needed right here.
	}
	FreeEuNumber(local);
#ifdef BITS64
	sprintf_s(buf, 80, "%lli", exp1);
#else
	sprintf_s(buf, 80, "%i", exp1);
#endif
	f += len;
	len = strlen(buf);
	len++;
	size = (size_t)(len + f);
	size++;
	ret = (char*)n1->data->myMalloc(size);
	if (ret == NULL)
	{
		ErrorMessageRanOutOfMemory();
	}
	memset(ret, 0, size);
	memcpy(ret, tmp, (size_t)f);
	n1->data->myFree(tmp);
	ret[f] = 'e';
	memcpy(ret + f + 1, buf, (size_t)len);
	return ret;
}

Float ToAtom(const Eun* a)
{
	char* st = ToString(a);
	char* stopstring;
	Float ret;
#ifdef FLOAT80
	ret = (Float)strtold(st, &stopstring);
#else
#ifdef FLOAT64
	ret = (Float)strtod(st, &stopstring);
#else
	ret = (Float)strtof(st, &stopstring);
#endif
#endif
	return ret;
}

// added functions:

Eun* IntPowerExp(EunInstanceData* d, Integer toPower, const IntegerArray* n1, 
	Integer exp1, Integer targetLength, Integer radix)
{
	Eun* tmp, * ret;
	Integer i;
	if (toPower == 0)
	{
		IntegerArray* a;
		a = NewIntegerArray(d, 1);
		*a->array = 1;
		ret = NewEuNumber(d, a, 0, targetLength, radix, 0);
		return ret;
	}
	ret = NewEuNumber(d, CopyIntegerArray(d, n1), exp1, targetLength, radix, 0);
	for (i = 2; i <= toPower; i++)
	{
		tmp = MultExp(d, ret->num, ret->exp, n1, exp1, targetLength, radix);
		FreeEuNumber(ret);
		ret = tmp;
	}
	return ret;
}

Eun* NthRootProtoExp(EunInstanceData* d, Integer n, const IntegerArray* x1,
	Integer x1Exp, const IntegerArray* guess,
	Integer guessExp, Integer targetLength, Integer radix)
{
	// function nth_root(object p, object guess, object n)
	//      object quotient, average
	//      quotient = p / power(guess, n-1)
	//      average = (quotient + ((n-1)* guess)) / n
	//      return average
	// end function
	Eun* p, * tmp, * quot, * average;
	IntegerArray a;
	Integer s[1];
	n--;
	p = IntPowerExp(d, n, guess, guessExp, targetLength, radix);
	quot = DivideExp(d, x1, x1Exp, p->num, p->exp, targetLength, radix);
	FreeEuNumber(p);
	*s = n;
	a.array = (Integer*)&s;
	a.len = 1;
	p = MultExp(d, &a, 0, guess, guessExp, targetLength, radix);
	tmp = AddExp(d, p->num, p->exp, quot->num, quot->exp, targetLength, radix, FALSE);
	FreeEuNumber(p);
	FreeEuNumber(quot);
	n++;
	*s = n;
	average = DivideExp(d, tmp->num, tmp->exp, &a, 0, targetLength, radix);
	FreeEuNumber(tmp);
	return average;
}

Eun* NthRootExp(EunInstanceData* d, Integer n, const IntegerArray* x1, Integer x1Exp,
	const IntegerArray* myguess, Integer guessExp, Integer targetLength, Integer radix)
{
	Eun* tmp, * lookat;
	IntegerArray* guess;
	Integer i, len, lastLen, protoTargetLength, moreAccuracy;
	if (x1->len <= 0)
	{
		d->lastNthRootIter = 1;
		tmp = NewEuNumber(d, CopyIntegerArray(d, x1), x1Exp, targetLength, radix, 0);
		return tmp;
	}
	if (x1->len == 1)
	{
		if ((*x1->array == 1) || (*x1->array == -1))
		{
			d->lastNthRootIter = 1;
			tmp = NewEuNumber(d, CopyIntegerArray(d, x1), x1Exp, targetLength, radix, 0);
			return tmp;
		}
	}
#ifdef FLOAT80
	moreAccuracy = (Integer)ceill(((Float)targetLength) / d->calcSpeed);
#else
#ifdef FLOAT64
	moreAccuracy = (Integer)ceil(((Float)targetLength) / d->calcSpeed);
#else
	moreAccuracy = (Integer)ceilf(((Float)targetLength) / d->calcSpeed);
#endif
#endif
	moreAccuracy++;
	targetLength += moreAccuracy;
	protoTargetLength = targetLength + moreAccuracy;
	guess = CopyIntegerArray(d, myguess);
	tmp = NewEuNumber(d, guess, guessExp, protoTargetLength, radix, 0);
	lastLen = guess->len;
	d->lastNthRootIter = maxIter;
	for (i = 1; i <= maxIter; i++)
	{
		lookat = tmp;
		tmp = NthRootProtoExp(d, n, x1, x1Exp, guess, guessExp, protoTargetLength, radix);
		guess = tmp->num;
		guessExp = tmp->exp;
		len = guess->len;
		if (len > targetLength)
		{
			len = targetLength;
		}
		if (len == lastLen)
		{
			if (guessExp == lookat->exp)
			{
				if (RangeArraysEqual(guess, lookat->num, 0, len - 1))
				{
					//if (d->lastNthRootIter == i - 1) {
					//	FreeEuNumber(lookat);
					//	break;
					//}
					d->lastNthRootIter = i;
					break;
				}
			}
		}
		FreeEuNumber(lookat);
		lastLen = len;
	}
	targetLength -= moreAccuracy;
	tmp->targetLength = targetLength;
	AdjustRound(tmp, TRUE);
	return tmp;
}

EunRetValues* EunNthRootGuessExp(Integer n, const Eun* in1, const Eun* guess, Integer exp1)
{
	// guess has to be positive on even roots
	Eun* n1, *ret;
	EunRetValues* retValues;
	Integer targetLength;
	unsigned int isImagFlag;
	n1 = (Eun*)in1;
	if (n1->data != guess->data)
	{
		return NULL;
	}
	if (n1->radix != guess->radix)
	{
		return NULL;
	}
	targetLength = (n1->targetLength >= guess->targetLength) ? n1->targetLength : guess->targetLength;
	isImagFlag = 0;
	if ((n & 1) == 0) // IsIntegerEven(n);
	{
		if (EunIsNegative(n1))
		{
			if (n1->data->realMode)
			{
				n1->data->myErrorCallBack(2);
			}
			// factor out sqrt(-1)
			n1 = EunNegate(in1);
			isImagFlag = 1;
		}
	}
	ret = NthRootExp(n1->data, n, n1->num, n1->exp, guess->num, guess->exp, targetLength, n1->radix);
	ret->imaginaryFlag = isImagFlag;
	exp1 /= n; // integer division: exp1 = (Integer)(exp1 / n);
	ret->exp += exp1;
	retValues = (EunRetValues*)n1->data->myMalloc(sizeof(EunRetValues));
	if (retValues == NULL)
	{
		ErrorMessageRanOutOfMemory();
	}
	if ((n & 1) == 0) // IsIntegerEven(n);
	{
		retValues->count = 2;
		retValues->values = (Eun**)n1->data->myMalloc(sizeof(void*) * 2);
		if (retValues->values == NULL)
		{
			ErrorMessageRanOutOfMemory();
		}
		*retValues->values = ret;
		retValues->values[1] = EunNegate(ret);
	}
	else
	{
		retValues->count = 1;
		retValues->values = (Eun**)n1->data->myMalloc(sizeof(void*));
		if (retValues->values == NULL)
		{
			ErrorMessageRanOutOfMemory();
		}
		*retValues->values = ret;
	}
	return retValues;
}

EunRetValues* EunNthRootGuess(Integer n, const Eun* in1, const Eun* guess)
{
	return EunNthRootGuessExp(n, in1, guess, 0);
}

EunRetValues* EunNthRoot(Integer n, const Eun* in1)
{
	// Latest code:
	Eun* n1 = CopyEun(in1);
	Integer exp1 = n1->exp;
	Integer f = exp1 % n; // remainder
	Float a;
	Integer isOdd;
	if (f > 0)
	{
		exp1 -= f;
		exp1 += (exp1 <= 0) ? n : 0;
	}
	n1->exp -= exp1;
	a = ToAtom(n1);
	FreeEuNumber(n1);
	isOdd = 0;
	if (a < 0)
	{
		// factor out sqrt(-1), an imaginary number, on even roots
		a *= -1;
		isOdd = ((n & 1) == 1);
	}
#ifdef FLOAT80
	a = (Float)powl(a, 1.0ld / (Float)n);
#else
#ifdef FLOAT64
	a = (Float)pow(a, 1.0 / (Float)n);
#else
	a = (Float)powf(a, 1.0f / (Float)n);
#endif
#endif
	if (isOdd != 0)
	{
		a *= -1;
	}
	n1 = ToEun(in1->data, a, in1->radix, in1->targetLength);
	return EunNthRootGuessExp(n, in1, n1, exp1);
}

// NOTE: There are other ways of doing square root,
//       including: x(k) = (1/2) ( x(k-1) + (n / ( x(k-1) )) )
//       which converges quadratically.
//       Maybe for a future version of MyEuNumber.

EunRetValues* EunSquareRoot(const Eun* n1)
{
	return EunNthRoot(2, n1);
}

EunRetValues* EunCubeRoot(const Eun* n1)
{
	return EunNthRoot(3, n1);
}

EunRetValues* EunSqrt(const Eun* n1)
{
	EunRetValues* retValues;
	Eun* guess, * local;
	Float a;
	Integer exp1 = n1->exp;
	// factor out a perfect square, of a power of radix, an even number (used later)
	if ((exp1 & 1) == 1) // if (IsIntegerOdd(exp1))
	{
		if (exp1 > 0)
		{
			exp1--;
		}
		else
		{
			exp1++;
		}
	}
	local = CopyEun(n1);
	local->exp -= exp1;
	a = ToAtom(local);
	if (a < 0)
	{
		a *= -1;
	}
#ifdef FLOAT80
	a = sqrtl(a);
#else
#ifdef FLOAT64
	a = sqrt(a);
#else
	a = sqrtf(a);
#endif
#endif
	guess = ToEun(local->data, a, local->radix, local->targetLength);
	retValues = EunNthRootGuess(2, local, guess);
	exp1 /= 2; // exp1 = exp1 / 2; // integer division
	(*retValues->values)->exp += exp1;
	retValues->values[1]->exp += exp1;
	FreeEuNumber(guess);
	FreeEuNumber(local);
	return retValues;
}


// Quadratic Equation yields two results
// ax^2 + bx + c
// f(a,b,c) = (-b +-sqrt(b*b - 4*a*c)) / (2*a)
// answer[0] = ((-b + sqrt(b*b - 4*a*c)) / (2*a))
// answer[1] = ((-b - sqrt(b*b - 4*a*c)) / (2*a))
QuadraticEquationRet* EunQuadraticEquation(const Eun* a, const Eun* b, const Eun* c)
{
	Eun* n0, * n1, * n2, * n3, * ans, * tmp, * tmp1;
	Complex* c0, * c1, * c2, * c3, * c4;
	QuadraticEquationRet* ret;
	EunRetValues* retValues;
	ComplexRetValues* complexValues;
	IntegerArray* f;
	if ((a->data != b->data) || (a->data != c->data)) {
		return NULL;
	}
	if ((a->radix != b->radix) || (a->radix != c->radix)) {
		return NULL;
	}
	if ((a->targetLength != b->targetLength) || (a->targetLength != c->targetLength)) {
		return NULL;
	}
	ans = EunMult(a, c); // a * c
	f = NewIntegerArray(a->data, 1);
	*f->array = 4;
	tmp = NewEuNumber(a->data, f, 0, a->targetLength, a->radix, 0);
	tmp1 = EunMult(tmp, ans); // 4 * a * c
	FreeEuNumber(tmp);
	FreeEuNumber(ans);
	Negate(tmp1->num); // - 4 * a * c
	tmp = EunMult(b, b); // b * b
	ans = EunAdd(tmp, tmp1); // ans = b * b - 4 * a * c
	FreeEuNumber(tmp);
	FreeEuNumber(tmp1);
	// To disallow sqrt of a negative number:
	//if ((ans->num->len > 0) && (*ans->num->array < 0))
	//{
	//	FreeEuNumber(ans);
	//	return NULL;
	//}
	// To allow complex number results:
	// +-sqrt(ans):
	retValues = EunSqrt(ans); // two (2) answers returned.
	FreeEuNumber(ans);
	tmp = EunNegate(b); // -b value
	ret = (QuadraticEquationRet*)a->data->myMalloc(sizeof(QuadraticEquationRet));
	if (ret == NULL)
	{
		ErrorMessageRanOutOfMemory();
	}
	if ((*retValues->values)->imaginaryFlag)
	{
		// Complex
		c0 = NewComplex(tmp, *retValues->values); // -b + sqrt(ans)i
		c1 = NewComplex(CopyEun(tmp), retValues->values[1]); // -b - sqrt(ans)i
		tmp = EunAdd(a, a); // 2a
		c2 = NewComplex(EunMultInv(tmp), NewEuNumber(a->data, NewIntegerArray(a->data, 0), 0, a->targetLength, a->radix, 0));
		c3 = ComplexMult(c0, c2); // (-b + sqrt(ans)i) / (2a)
		c4 = ComplexMult(c1, c2); // (-b - sqrt(ans)i) / (2a)
		a->data->myFree(retValues->values);
		a->data->myFree(retValues);
		FreeComplex(c0);
		FreeComplex(c1);
		FreeEuNumber(tmp);
		FreeComplex(c2);
		complexValues = (ComplexRetValues*)a->data->myMalloc(sizeof(ComplexRetValues));
		if (complexValues == NULL)
		{
			ErrorMessageRanOutOfMemory();
		}
		complexValues->count = 2;
		complexValues->values = (Complex**)a->data->myMalloc(sizeof(void*) * (size_t)complexValues->count);
		if (complexValues->values == NULL)
		{
			ErrorMessageRanOutOfMemory();
		}
		*complexValues->values = c3;
		complexValues->values[1] = c4;
		ret->isComplex = 1;
		ret->values.complexValues = complexValues;
		return ret;
	}
	else
	{
		n2 = *retValues->values;
		n3 = retValues->values[1];
		n0 = EunAdd(tmp, n2); // -b + sqrt(ans)
		FreeEuNumber(n2);
		n1 = EunAdd(tmp, n3); // -b - sqrt(ans)
		FreeEuNumber(n3);
		FreeEuNumber(tmp);
		tmp = EunAdd(a, a); // 2a
		ans = EunMultInv(tmp); // 1/(2a)
		n2 = EunMult(n0, ans); // (-b + sqrt(ans)) / (2a)
		n3 = EunMult(n1, ans); // (-b - sqrt(ans)) / (2a)
		FreeEuNumber(n0);
		FreeEuNumber(n1);
		FreeEuNumber(tmp);
		FreeEuNumber(ans);
		//NOTE: return n2 and n3
		*retValues->values = n2;
		retValues->values[1] = n3;
		ret->isComplex = 0;
		ret->values.eunValues = retValues;
		return ret;
	}
}

// Complex functions:

Complex* NewComplex(Eun* real, Eun* imag)
{
	Complex* ret;
	if (real->data != imag->data)
	{
		return NULL;
	}
	ret = (Complex*)real->data->myMalloc(sizeof(Complex));
	if (ret == NULL)
	{
		ErrorMessageRanOutOfMemory();
	}
	if (real->imaginaryFlag == 1)
	{
		real->imaginaryFlag = 0;
	}
	ret->real = real;
	if (imag->imaginaryFlag == 0)
	{
		imag->imaginaryFlag = 1;
	}
	ret->imag = imag;
	return ret;
}

void FreeComplex(Complex* a)
{
	if (a != NULL)
	{
		EunInstanceData* d = a->real->data;
		FreeEuNumber(a->real);
		FreeEuNumber(a->imag);
		d->myFree(a);
	}
}

void NegateImagPart(Complex* a)
{
	Negate(a->imag->num);
}

Complex* ComplexAdd(const Complex* a, const Complex* b)
{
	Eun* real, * imag;
	Complex* ret;
	real = EunAdd(a->real, b->real);
	imag = EunAdd(a->imag, b->imag);
	ret = NewComplex(real, imag);
	return ret;
}

Complex* ComplexNegate(const Complex* b)
{
	Eun* real, * imag;
	Complex* ret;
	real = EunNegate(b->real);
	imag = EunNegate(b->imag);
	ret = NewComplex(real, imag);
	return ret;
}

Complex* ComplexSubtract(const Complex* a, const Complex* b)
{
	Eun* real, * imag;
	Complex* ret;
	real = EunSubtract(a->real, b->real);
	imag = EunSubtract(a->imag, b->imag);
	ret = NewComplex(real, imag);
	return ret;
}

Complex* ComplexMult(const Complex* n1, const Complex* n2)
{
	// i = sqrt(-1)
	// n1 = (a+bi)
	// n2 = (c+di)
	// (a+bi)(c+di) <=> ac + adi + bci + bdi*i
	// <=> (ac - bd) + (ad + bc)i
	Eun* real, * imag, * a, * b;
	Complex* ret;
	a = EunMult(n1->real, n2->real);
	b = EunMult(n1->imag, n2->imag);
	real = EunSubtract(a, b);
	FreeEuNumber(a);
	FreeEuNumber(b);
	a = EunMult(n1->real, n2->imag);
	b = EunMult(n1->imag, n2->real);
	imag = EunAdd(a, b);
	FreeEuNumber(a);
	FreeEuNumber(b);
	ret = NewComplex(real, imag);
	return ret;
}

Complex* ComplexMultInv(const Complex* n2)
{
	// Eun a, b, c
	// (a + bi)(a - bi) <=> a * a + b * b
	// n2 = (a + bi)
	// a = n2[1]
	// b = n2[2]
	// c = (a * a + b * b)
	// 1 / n2 <=> (a - bi) / (a * a + b * b)
	// <=> (a / (a * a + b * b)) - (b / (a * a + b * b))i
	// <=> (a / c) - (b / c)i
	Eun* a, * b, * c;
	Complex* ret;
	a = EunMult(n2->real, n2->real);
	c = EunMult(n2->imag, n2->imag);
	b = EunAdd(a, c);
	FreeEuNumber(a);
	FreeEuNumber(c);
	c = EunMultInv(b);
	FreeEuNumber(b);
	a = EunMult(n2->real, c);
	b = EunMult(n2->imag, c);
	FreeEuNumber(c);
	Negate(b->num);
	ret = NewComplex(a, b);
	return ret;
}

Complex* ComplexDivide(const Complex* n1, const Complex* n2)
{
	Complex* ret, * tmp;
	tmp = ComplexMultInv(n2);
	ret = ComplexMult(n1, tmp);
	FreeComplex(tmp);
	return ret;
}

// This equation takes REAL numbers as input to "p" and "y"
// So, they use the NON - complex functions to calculate them.
// sqrt(p + iy) <=> (1 / 2) * sqrt(2) * [sqrt(sqrt(p * p + y * y) + p) + i * sign(y) * sqrt(sqrt(p * p + y * y) - p)]
//
// NOTE: results are both positive and negative. Remember "i" (the imaginary part) is always both positive and negative in mathematics.
// NOTE: So, you will need to keep in mind that information in your equations.
ComplexRetValues* ComplexSqrt(const Complex* a)
{
	Eun* x, * y, * n1, * n2, * tmp, * tmptwo;
	Complex* c;
	EunRetValues* s;
	ComplexRetValues* ret;
	Integer moreAccuracy;
	EunInstanceData* d;
	x = CopyEun(a->real);
	y = CopyEun(a->imag);
	d = x->data;
#ifdef FLOAT80
	moreAccuracy = (Integer)ceill(((Float)x->targetLength) / d->calcSpeed);
#else
#ifdef FLOAT64
	moreAccuracy = (Integer)ceil(((Float)x->targetLength) / d->calcSpeed);
#else
	moreAccuracy = (Integer)ceilf(((Float)x->targetLength) / d->calcSpeed);
#endif
#endif
	moreAccuracy++;
	x->targetLength += moreAccuracy;
	y->targetLength += moreAccuracy;
	n1 = EunMult(x, x);
	n2 = EunMult(y, y);
	tmp = EunAdd(n1, n2);
	FreeEuNumber(n1);
	FreeEuNumber(n2);
	s = EunSqrt(tmp); // shouldn't return an imaginary number
	FreeEuNumber(tmp);
	FreeEuNumber(s->values[1]); // free the negative answer
	tmp = *s->values; // get the postive answer
	tmp->data->myFree(s);
	n1 = EunAdd(tmp, x);
	n2 = EunSubtract(tmp, x);
	FreeEuNumber(tmp);
	FreeEuNumber(x);
	//n1
	s = EunSqrt(n1);
	FreeEuNumber(n1);
	FreeEuNumber(s->values[1]);
	n1 = *s->values; // get the postive answer
	if (n1->imaginaryFlag == 1)
	{
		n1->data->myErrorCallBack(5); // Error: something went wrong
	}
	n1->data->myFree(s);
	//n2
	s = EunSqrt(n2);
	FreeEuNumber(n2);
	FreeEuNumber(s->values[1]);
	n2 = *s->values; // get the postive answer
	if (n2->imaginaryFlag == 1)
	{
		n2->data->myErrorCallBack(5); // Error: something went wrong
	}
	n2->data->myFree(s);
	if (EunIsNegative(y))
	{
		Negate(n2->num);
	}
	FreeEuNumber(y);
	//tmp and tmptwo
	tmptwo = NewEuNumber(d, IntegerArrayFromIntegers(d, valueOfTwo, 1), 0, x->targetLength, x->radix, 0);
	s = EunSqrt(tmptwo); // tmptwo
	//FreeEuNumber(tmptwo); // used later
	FreeEuNumber(s->values[1]);
	tmp = *s->values; // get the postive answer
	//if (tmp->imaginaryFlag == 1)
	//{
	//	ErrorMessage(); // Error: something went wrong
	//}
	tmp->data->myFree(s);
	x = tmp;
	tmp = EunDivide(x, tmptwo);
	FreeEuNumber(x);
	FreeEuNumber(tmptwo);
	x = n1;
	n1 = EunMult(x, tmp); // n1 * tmp;
	FreeEuNumber(x);
	y = n2;
	n2 = EunMult(y, tmp); // n2 * tmp;
	FreeEuNumber(y);
	FreeEuNumber(tmp);
	n1->targetLength -= moreAccuracy;
	n2->targetLength -= moreAccuracy;
	AdjustRound(n1, TRUE);
	AdjustRound(n2, TRUE);
	// return the results:
	c = NewComplex(n1, n2);
	ret = (ComplexRetValues*)d->myMalloc(sizeof(ComplexRetValues));
	if (ret == NULL)
	{
		ErrorMessageRanOutOfMemory();
	}
	ret->count = 2;
	ret->values = (Complex**)d->myMalloc(sizeof(void*) * (size_t)ret->count);
	if (ret->values == NULL)
	{
		ErrorMessageRanOutOfMemory();
	}
	*ret->values = c;
	ret->values[1] = ComplexNegate(c);
	return ret;
}

//  About the Quadratic Equation :
//  
//  The quadratic equation produces two answers (the two answers may be the same)
//  ax ^ 2 + bx + c
//   f(a, b, c) = (-b + -sqrt(b * b - 4 * a * c)) / (2 * a)
//  answer[0] = ((-b + sqrt(b * b - 4 * a * c)) / (2 * a))
//  answer[1] = ((-b - sqrt(b * b - 4 * a * c)) / (2 * a))
//  
//  The "Complex" quadratic equation produces about 2 results
//  
ComplexRetValues* ComplexQuadraticEquation(const Complex* a, const Complex* b, const Complex* c)
{
	Complex* n1, * n2, * ans, * tmp, * n3;
	ComplexRetValues* s;
	IntegerArray* f;
	Eun* p1 = a->real, * p2 = a->imag;
	ans = ComplexMult(a, c);
	f = NewIntegerArray(p1->data, 1);
	*f->array = 4;
	p1 = NewEuNumber(p1->data, f, 0, p1->targetLength, p1->radix, 0);
	p2 = NewEuNumber(p2->data, NewIntegerArray(p2->data, 0), 0, p2->targetLength, p2->radix, 0);
	tmp = NewComplex(p1, p2);
	n3 = ComplexMult(tmp, ans);
	FreeComplex(tmp);
	FreeComplex(ans);
	tmp = ComplexMult(b, b);
	ans = ComplexSubtract(tmp, n3);
	FreeComplex(tmp);
	FreeComplex(n3);
	s = ComplexSqrt(ans);
	FreeComplex(ans);
	ans = *s->values;
	n3 = s->values[1];
	tmp = ComplexNegate(b);
	n1 = ComplexAdd(ans, tmp);
	n2 = ComplexAdd(n3, tmp);
	FreeComplex(tmp);
	FreeComplex(ans);
	FreeComplex(n3);
	n3 = ComplexAdd(a, a);
	ans = ComplexMultInv(n3);
	FreeComplex(n3);
	*s->values = ComplexMult(n1, ans);
	s->values[1] = ComplexMult(n2, ans);
	FreeComplex(ans);
	FreeComplex(n1);
	FreeComplex(n2);
	return s;
}

// END OF FILE.
