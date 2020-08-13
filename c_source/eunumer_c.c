//
// eunumer_c.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "myeunumber.h"


int main()
{
	EunInstanceData myinst;
	Eun* n1, * n2, * ans;
	EunRetValues* retVals;
	Float a = 1.0;
	char* str;
	char inbuf[10000]; // ten thousand bytes buffer for input

	EunInit();
	EunInitInstance(&myinst, sizeof(EunInstanceData), &malloc, &free, NULL);

	//myinst.defaultRadix = 9;
	//myinst.defaultTargetLength = 200;
	//myinst.roundingMethod = ROUND_ODD;

	printf("MyEunumber for C [Version %i]\n", EunVersion());

	while (1)
	{
		printf("Enter a string number, like: -123.456e-123\n");
#ifdef unix
		scanf("%s", (char *)&inbuf);
#else
		scanf_s("%s", &inbuf, (unsigned)_countof(inbuf));
#endif
		n1 = StringToEun(&myinst, (const char*)&inbuf, 0, 0); // use default radix and targetLength
		a = ToAtom(n1);
		printf("You entered: %g\n", a);
		EunPrint(n1);

		printf("Enter another string number, or zero (0) to exit:\n");
#ifdef unix
		scanf("%s", (char *)&inbuf);
#else
		scanf_s("%s", &inbuf, (unsigned)_countof(inbuf));
#endif
		n2 = StringToEun(&myinst, (const char*)&inbuf, 0, 0); // use default radix and targetLength
		a = ToAtom(n2);
		printf("You entered: %g\n", a);
		EunPrint(n2);

		printf("Addition: \n");

		ans = EunAdd(n1, n2);
		EunPrint(ans);
		FreeEuNumber(ans);

		printf("Multiplication: \n");

		ans = EunMult(n1, n2);
		EunPrint(ans);

		if (a == 0.0)
		{
			break;
		}

		FreeEuNumber(ans);
		printf("Division: \n");

		ans = EunDivide(n1, n2);
		EunPrint(ans);
		//FreeEuNumber(ans);

		printf("Division value is: ");
		str = ToString(ans);
		printf("%s\n", str);
		ans->data->myFree(str);

		//if (a == 0.0)
		//{
		//	break;
		//}

		printf("Square root of last value (ans): \n");

		retVals = EunSquareRoot(ans);
		FreeEuNumber(n1);
		FreeEuNumber(n2);
		n1 = retVals->values[0];
		n2 = retVals->values[1];
		ans->data->myFree(retVals);
		EunPrint(n1);
		EunPrint(n2);

		if (n1->imaginaryFlag)
		{
			printf("Square root value results are imaginary numbers.\n");
		}
		else
		{
			printf("Square root value results are real numbers.\n");
		}

		printf("Positive Sqrt(ans) Value is: \n+");
		str = ToString(n1);
		printf("%s\n", str);
		ans->data->myFree(str);

		printf("Negative Sqrt(ans) Value is: \n");
		str = ToString(n2);
		printf("%s\n", str);
		ans->data->myFree(str);

		//if (a == 0.0)
		//{
		//	break;
		//}

		FreeEuNumber(ans);
		FreeEuNumber(n1);
		FreeEuNumber(n2);

	}
	
	FreeEuNumber(ans);
	FreeEuNumber(n1);
	FreeEuNumber(n2);

	puts("done.\n");
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to Add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to Add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
