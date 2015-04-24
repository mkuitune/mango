#include "Commons/cms_math.h"
#include <iostream>

int main(int argc, char* argv [])
{
	cms::Vec3f a = {1.f, 2.f, 3.f};
	auto b = a + a;

	std::cout << "I got" << b << std::endl;

	return 0;
}
