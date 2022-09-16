#pragma once
#include <cstddef>
#include <array>
#include <stdexcept>
#include <cassert>
#include <type_traits>
#include <new>

namespace ez {
	template<typename T, size_t N>
	class SmallVec {
	public:
		using self_t = SmallVec<T, N>;
		using container_t = std::array<T, N>;

		using value_type = T;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;

		using iterator = typename container_t::iterator;
		using const_iterator = typename container_t::const_iterator;
		using reverse_iterator = typename container_t::reverse_iterator;
		using const_reverse_iterator = typename container_t::const_reverse_iterator;

		// Empty constructor, starts at size 0
		SmallVec()
			: mcount(0)
		{
			std::memset(arr, 0, sizeof(arr));
		}
		// Deconstruct
		~SmallVec() {
			for (size_type i = 0; i < mcount; ++i) {
				(&container[i])->~T();
			}
			mcount = 0;
		}

		// Move construct
		SmallVec(SmallVec&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
			: mcount(other.mcount)
		{
			for (size_type i = 0; i < mcount; ++i) {
				new (&container[i]) T(std::move(other.container[i]));
			}
			other.mcount = 0;
		}
		// Copy construct
		SmallVec(const SmallVec& other) noexcept(std::is_nothrow_copy_constructible_v<T>)
			: mcount(other.mcount)
		{
			for (size_type i = 0; i < mcount; ++i) {
				new (&container[i]) T(other.container[i]);
			}
		}
		SmallVec& operator=(const SmallVec& other) noexcept(std::is_nothrow_copy_assignable_v<T>) {
			this->~self_t();
			new (this) self_t(other);

			return *this;
		}
		SmallVec& operator=(SmallVec&& other) noexcept(std::is_nothrow_move_assignable_v<T>) {
			this->~self_t();
			new (this) self_t(std::move(other));

			return *this;
		}
		SmallVec(std::initializer_list<T> ilist) noexcept(std::is_nothrow_constructible_v<T>)
			: mcount(ilist.size())
		{
			assert(size() <= max_size());
			for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first) {
				push_back(*first);
			}
		}
		SmallVec& operator=(std::initializer_list<T> ilist) noexcept(std::is_nothrow_constructible_v<T>) {
			this->~self_t();
			new (this) self_t(ilist);
			return *this;
		}

		constexpr size_type size() const noexcept {
			return mcount;
		}
		constexpr size_type max_size() const noexcept {
			return N;
		}
		constexpr size_type capacity() const noexcept {
			return max_size();
		}
		constexpr bool empty() const noexcept {
			return mcount == 0;
		}

		constexpr reference at(size_type pos) {
			if (pos >= size()) {
				throw std::out_of_range("index out of bounds!");
			}
			return container[pos];
		}
		constexpr const_reference at(size_type pos) const {
			if (pos >= size()) {
				throw std::out_of_range("index out of bounds!");
			}
			return container[pos];
		}

		constexpr reference operator[](size_type pos) {
			assert(pos < size());
			return container[pos];
		}
		constexpr const_reference operator[](size_type pos) const {
			assert(pos < size());
			return container[pos];
		}

		constexpr reference front() {
			assert(!empty());
			return container[0];
		}
		constexpr const_reference front() const {
			assert(!empty());
			return container[0];
		}

		constexpr reference back() {
			assert(!empty());
			return container[mcount - 1];
		}
		constexpr const_reference back() const {
			assert(!empty());
			return container[mcount - 1];
		}
		
		void swap(self_t& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
			self_t tmp(std::move(*this));
			new (this) self_t(std::move(other));
			new (&other) self_t(std::move(tmp));
		}

		pointer data() noexcept {
			return &container[0];
		}
		const_pointer data() const noexcept {
			return &container[0];
		}

		iterator begin() noexcept {
			return container.begin();
		}
		const_iterator begin() const noexcept {
			return container.cbegin();
		}

		iterator end() noexcept {
			return container.begin() + mcount;
		}
		const_iterator end() const noexcept {
			return container.cbegin() + mcount;
		}

		reverse_iterator rbegin() noexcept {
			return std::make_reverse_iterator(end());
		}
		const_reverse_iterator rbegin() const noexcept {
			return std::make_reverse_iterator(end());
		}

		reverse_iterator rend() noexcept {
			return std::make_reverse_iterator(begin());
		}
		const_reverse_iterator rend() const noexcept {
			return std::make_reverse_iterator(begin());
		}

		const_reverse_iterator crbegin() const noexcept {
			return std::make_reverse_iterator(end());
		}
		const_reverse_iterator crend() const noexcept {
			return std::make_reverse_iterator(begin());
		}

		void push_back(const T& value) {
			assert(size() < max_size());
			new (&container[mcount]) T(value);
			++mcount;
		}
		void push_back(T&& value) {
			assert(size() < max_size());
			new (&container[mcount]) T(std::move(value));
			++mcount;
		}
		void pop_back() {
			assert(!empty());
			(&container[mcount - 1])->~T();
			--mcount;
		}
		void clear() {
			for (size_type i = 0; i < mcount; ++i) {
				(&container[i])->~T();
			}
			mcount = 0;
		}

		template<typename... Args>
		void emplace_back(Args&&... args) {
			assert(size() < max_size());
			new (&container[mcount]) T(std::forward<Args>(args)...);
			++mcount;
		}

		void assign(std::initializer_list<T> ilist) noexcept(std::is_nothrow_constructible_v<T>) {
			assert(ilist.size() <= max_size());
			clear();
			for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first) {
				push_back(*first);
			}
		}
		template<typename input_iter, 
			std::enable_if_t<
				!std::is_same_v<std::remove_const_t<std::remove_reference_t<input_iter>>, 
				std::remove_const_t<std::remove_reference_t<T>>>
			, int> = 0>
		void assign(input_iter first, input_iter last) {
			clear();
			for (; first != last; ++first) {
				push_back(*first);
			}
		}
		void assign(size_type num, const T& value) noexcept(std::is_nothrow_constructible_v<T>) {
			assert(num <= max_size());
			clear();
			for (size_t i = 0; i < num; ++i) {
				push_back(value);
			}
		}

		template<std::enable_if_t<std::is_default_constructible_v<T>, int> = 0>
		void resize(size_t count) {
			assert(count <= max_size());
			if (count < size()) {
				for (size_t i = 0, rem = size() - count; i < rem; ++i) {
					pop_back();
				}
			}
			else {
				for (size_t i = size(); i < count; ++i) {
					emplace_back();
				}
			}
		}
		void resize(std::size_t count, const value_type& value) noexcept(std::is_nothrow_copy_constructible_v<T>) {
			assert(count <= max_size());
			if (count < size()) {
				for (size_t i = 0, rem = size() - count; i < rem; ++i) {
					pop_back();
				}
			}
			else {
				for (size_t i = size(); i < count; ++i) {
					emplace_back(value);
				}
			}
		}

		iterator insert(const_iterator pos, const T& value) {
			assert(size() < max_size());
			assert(pos < end() && pos >= begin());

			T* ptr = data() + (pos - begin());
			T* last = data() + size(); // The last VALID position to write to

			// Move old data forward an index, to make room for the new value
			for (T * mvptr = last; mvptr != ptr; --mvptr) {
				if constexpr (std::is_move_constructible_v<T>) {
					new (mvptr) T(std::move(*(mvptr - 1)));
				}
				else {
					// Fallback to copy construct
					new (mvptr) T(*(mvptr - 1));
				}
			}
			// Now just insert
			new (ptr) T(value);
			++mcount;

			return begin() + (pos - begin() + 1);
		}
		iterator insert(const_iterator pos, T&& value) {
			assert(size() < max_size());
			assert(pos < end() && pos >= begin());

			T* ptr = data() + (pos - begin());
			T* last = data() + size(); // The last VALID position to write to

			// Move old data forward an index, to make room for the new value
			for (T* mvptr = last; mvptr != ptr; --mvptr) {
				if constexpr (std::is_move_constructible_v<T>) {
					new (mvptr) T(std::move(*(mvptr - 1)));
				}
				else {
					// Fallback to copy construct
					new (mvptr) T(*(mvptr - 1));
				}
			}

			if constexpr (std::is_move_constructible_v<T>) {
				new (ptr) T(std::move(value));
			}
			else {
				// Fallback to copy construct
				new (ptr) T(value);
			}
			++mcount;

			return begin() + (pos - begin() + 1);
		}
		void insert(const_iterator pos, size_t count, const T& value) {
			assert((size() + count) <= max_size());
			assert(pos < end() && pos >= begin());

			T* first = data() + (pos - begin());
			T* ptr = first + count;
			T* last = data() + (size() + count - 1); // The last VALID position to write to

			// Move old data forward by count indices, to make room for the new values
			for (T* mvptr = last; mvptr != ptr; --mvptr) {
				if constexpr (std::is_move_constructible_v<T>) {
					new (mvptr) T(std::move(*(mvptr - count)));
				}
				else {
					// Fallback to copy construct
					new (mvptr) T(*(mvptr - count));
				}
			}
			// Now just copy insert
			for (size_t i = 0; i < count; ++i) {
				new (first++) T(value);
			}
			mcount += count;
		}
		iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
			assert((size() + ilist.size()) <= max_size());
			assert(pos < end() && pos >= begin());

			size_t count = ilist.size();
			T* first = data() + (pos - begin());
			T* ptr = first + count;
			T* last = data() + (size() + count - 1); // The last VALID position to write to

			// Move old data forward by count indices, to make room for the new values
			for (T* mvptr = last; mvptr != ptr; --mvptr) {
				if constexpr (std::is_move_constructible_v<T>) {
					new (mvptr) T(std::move(*(mvptr - count)));
				}
				else {
					// Fallback to copy construct
					new (mvptr) T(*(mvptr - count));
				}
			}

			for (auto it = ilist.begin(), lit = ilist.end(); it != lit; ++it) {
				new (first++) T(*it);
			}
			mcount += count;

			return begin() + ((pos - begin()) + count);
		}

		template<typename ... Args>
		iterator emplace(const_iterator pos, Args&&... args) {
			assert(size() < max_size());
			assert(pos < end() && pos >= begin());

			T* ptr = const_cast<T*>(&*pos);
			T* last = data() + size(); // The last VALID position to write to

			// Move old data forward an index, to make room for the new value
			for (T* mvptr = last; mvptr != ptr; --mvptr) {
				if constexpr (std::is_move_constructible_v<T>) {
					new (mvptr) T(std::move(*(mvptr - 1)));
				}
				else {
					// Fallback to copy construct
					new (mvptr) T(*(mvptr - 1));
				}
			}

			new (ptr) T(std::forward<Args>(args)...);
			++mcount;

			return begin() + (pos - begin());
		}

		iterator erase(const_iterator pos) {
			assert(pos >= begin() && pos < end());

			T* ptr = const_cast<T*>(&*pos);
			T* eptr = data() + size()-1; // The last VALID position to write to

			// Destruct the object at pos
			ptr->~T();

			// Move old data back an index, filling the gap
			for (T* write = ptr; write != eptr; ++write) {
				if constexpr (std::is_move_constructible_v<T>) {
					new (write) T(std::move(*(write + 1)));
				}
				else {
					// Fallback to copy construct
					new (write) T(*(write + 1));
				}
			}
			--mcount;
			return begin() + (ptr - data());
		}
		iterator erase(const_iterator first, const_iterator last) {
			assert(first >= begin() && first < end());
			assert(last >= begin() && last <= end());
			assert(first <= last);

			T* fptr = const_cast<T*>(&*first);
			T* lptr = const_cast<T*>(&*last);
			T* eptr = data() + size();

			// Destruct the objects
			for (T* ptr = fptr; ptr != lptr; ++ptr) {
				ptr->~T();
			}

			// Shift the remaining objects to fill the space
			for (T* write = fptr, *read = lptr; read != eptr; ++write, ++read) {
				if constexpr (std::is_move_constructible_v<T>) {
					new (write) T(std::move(*read));
				}
				else {
					// Fallback to copy construct
					new (write) T(*read);
				}
			}
			mcount -= (lptr - fptr);
			return begin() + (fptr - data());
		}
	private:
		union {
			container_t container;
			char arr[sizeof(container_t)];
		};
		std::size_t mcount;
	};
}