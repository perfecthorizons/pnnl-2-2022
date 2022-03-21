
#include "../vectornav.h"
#include <thread>
#include <chrono>

int main(int argc, char *argv[]) {

	VectorNav vn;
	VNSM *vnsm = vn.get_shared_memory();

	while (true) {
		std::cout << "Roll: " << vnsm->read<IDX_ROLL>();
		//std::cout << " Pitch: " << vnsm->read<IDX_PITCH>();
	        std::cout << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

}

