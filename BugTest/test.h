#if !defined(TEST_H)
#define TEST_H

namespace Test
{
	class A
	{
	public:
		//A(void) = default;
		A(int param) {}
	};

	class B
	{
	public:
		B(void) = default;
		A a;
	};

}
#endif
