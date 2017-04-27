#include "FastAllocator.h"
#include <iostream>

const size_t SIZE_IN_OBJECTS = 10;

int pool[SIZE_IN_OBJECTS];

int main() {
	fastallocator::FastAllocator<int> alloc(pool, SIZE_IN_OBJECTS);

	auto pointer = alloc.allocate();
	*pointer = 5;
	alloc.deallocate(pointer);

	try {
		alloc.deallocate(nullptr);
	}
	catch (std::invalid_argument e) {
		std::cout << e.what() << std::endl;
	}

	for (auto i = 0; i < SIZE_IN_OBJECTS + 1; i++) {
		try {
			alloc.allocate();
		}
		catch (std::bad_alloc e) {
			// Should happen on i == SIZE_IN_OBJECTS
			std::cout << e.what() << " - element " << i << std::endl;
		}

	}

	std::cout << "Everything OK" << std::endl;
}
