#pragma once
namespace cms{
	unsigned char* aligned_alloc(size_t size);
	void aligned_free(void* p);


	/** Manage a static array of data aligned at 16 byte boundary. */
	template<class T>
	class AlignedArray
	{
	public:
		typedef T  value_type;
		typedef T* iterator;
		typedef const T* const_iterator;


		AlignedArray() :data_(nullptr), capacity_(0), size_(0)
		{
			// Make sure T is aligned to 16 bytes
			static_assert((sizeof(T) == 1) ||
				(sizeof(T) < 8 && sizeof(T) % 2 == 0) ||
				(sizeof(T) % 16 == 0),
				"Size of stored datatype is not alignable to 16 bytes!");
			size_t hint = 12;
			realloc_data(hint);
		}

		AlignedArray(size_t count, const T& value = T()) :data_(nullptr), capacity_(0), size_(0)
		{
			resize(count, value);
		}


		AlignedArray(const AlignedArray& old)
		{
			*this = old;
		}

		AlignedArray(AlignedArray&& old){
			data_ = old.data_;
			capacity_ = old.capacity_;
			size_ = old.size_;
			old.data_ = 0;
			old.capacity_ = 0;
			old.size_ = 0;
		}

		~AlignedArray()
		{
			if (data_) aligned_free(data_);
		}

		AlignedArray& operator=(const AlignedArray& old)
		{
			realloc_data(old.capacity());
			unsafe_copy(old.begin(), old.end(), data_);
			size_ = old.size();
		}

		AlignedArray& operator=(AlignedArray&& old)
		{
			if (this != &old)
			{
				data_ = old.data_;
				capacity_ = old.capacity_;
				size_ = old.size_;
				old.data_ = 0;
				old.capacity_ = 0;
				old.size_ = 0;
			}
			return *this;
		}

		T& operator[](size_t index){ return data_[index]; }

		T* data(){ return data_; }

		T& append()
		{
			add_data(1);
			return &data_[size_ - 1];
		}

		void resize(size_t new_size, const T& init_value)
		{
			realloc_data(new_size);
			if (new_size > size_)
			{
				size_t delta = new_size - size_;
				size_t oldSize = size_;
				add_data(delta);
				for (size_t i = oldSize; i < new_size; ++i) data_[i] = init_value;
			}
		}

		iterator begin() { return data_; }
		iterator end() { return data_ + size_; }

		const_iterator begin() const { return data_; }
		const_iterator end() const { return data_ + size_; }


		size_t size() const { return size_; }
		size_t capacity() const { return capacity_; }
		size_t value_size() const { return sizeof(T); }

		void assign(T* start_ptr, T* end_ptr)
		{
			size_t count = end_ptr - start_ptr;
			size_ = 0;
			add_data(count);
			unsafe_copy(start_ptr, end_ptr, data_);
		}
		void clear() { size_ = 0; }
	private:

		void add_data(size_t count)
		{
			size_t new_size = size_ + count;
			if (new_size > capacity_)
			{
				auto new_capacity = capacity_ * 2;
				if (new_size > new_capacity) new_capacity = new_size + new_size / 5;

				realloc_data(new_capacity);
			}

			size_ = new_size;
		}

		void realloc_data(size_t new_capacity)
		{
			if (new_capacity <= capacity_) return;

			size_t new_size_bytes = count_in_bytes(new_capacity);

			if (data_)
			{
				T* new_data = (T*) aligned_alloc(new_size_bytes);
				unsafe_copy(begin(), end(), new_data);
				aligned_free(data_);
				data_ = new_data;
			}
			else
			{
				data_ = (T*) aligned_alloc(new_size_bytes);
				memset(data_, 0, new_size_bytes);
			}

			capacity_ = new_capacity;
		}

		size_t count_in_bytes(size_t count){ return count * value_size(); }

		T* data_;
		size_t capacity_;
		size_t size_;
	};
}