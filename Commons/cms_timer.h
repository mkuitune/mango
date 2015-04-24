#pragma once
#include <chrono>
namespace cms {
	class Timer {
		typedef std::chrono::high_resolution_clock high_resolution_clock;
		typedef std::chrono::milliseconds milliseconds;
	public:
		
		high_resolution_clock::time_point start;
		explicit Timer(bool run = true){
			if (run)
				reset();
		}

		void reset(){
			start = high_resolution_clock::now();
		}

		milliseconds elapsed() const {
			return std::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - start);
		}

		template <typename T, typename Traits>
		friend std::basic_ostream<T, Traits>& operator<<(std::basic_ostream<T, Traits>& out, const Timer& timer){
			return out << timer.elapsed().count();
		}
	};

}