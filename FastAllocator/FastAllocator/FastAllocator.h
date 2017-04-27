#ifndef _fast_allocator_h_
#define _fast_allocator_h_

#include <memory> // bad_alloc
#include <stdexcept> // invalid_argument

namespace fastallocator {

	/**
	* \brief Class implementing the Fast allocator
	* \details Standard C++ allocator is pretty fast, but
	*          when there is a need only for objects of certain
	*          type, and not arrays, allocation can be optimized.
	*          Besides that, allocation can be quicker if it does
	*          not happen on the heap. This allocator uses whatever
	*          memory is already allocated and passed to it, and
	*          allocates objects from there. So, one could allocate
	*          a static global array of bytes, and pass it to this
	*          allocator. This might be a little bit quicker than
	*          heap allocation.
	*          
	*          The allocator is implemented using the slab allocator as
	*          an idea, and it behaves as if the whole memory used is one
	*          slab. Internally, the allocator keeps a list of free objects,
	*          and when an objects should be allocated, the allocator removes
	*          one object from the list, and returns it. When an object is
	*          deallocated, the allocator simply returns that object to the
	*          list of free objects.
	*                    
	* \author Nikola Bebic
	* \version 1.0.0.0.
	* \date 27-Apr-2017
	* \copyright MIT Licence
	*/
	template <typename T> class FastAllocator {
	public:

		/**
		 * \brief Construct the fast allocator object
		 * \param memory_start Pointer to the beggining of the byte array that the allocator should use
		 * \param size_in_objects Size of the byte array expressed in \c sizeof(T) units
		 */
		FastAllocator(void *memory_start, size_t size_in_objects);

		/**
		 * \brief Allocates one object
		 * \return Pointer to the allocated object
		 */
		T* allocate();

		/**
		 * \brief Deallocates one object
		 * \param object Pointer to the object previously obtained by \c allocate()
		 */
		void deallocate(T* object);

	private:

		/**
		 * \brief Union representing one object that can be allocated
		 * \details The union is used because objects of size \c sizeof(T) must have 
		 *          pointers to the next such object. 
		 */
		union Element {
			Element *next_; /**< Pointer to the next element in the list of free elements */
			T value_; /**< Real value of the element */

			/**
			 * \brief Construct one element of the list
			 * \param next Pointer to the next element in list
			 */
			explicit Element(Element *next = nullptr) {
				next_ = next;
			}
		};

		void *memory_start_; /**< Pointer to the beggining of the byte array used by allocator */
		size_t size_in_objects_; /**< Size of the byte array expressed in \c sizeof(T) units */
		Element *first_element_; /**< Pointer to the first free element, or \c nullptr if no such element exists */

		/**
		 * \brief Initializes the free elements list in the allocator
		 */
		void initialize();

	};

	template<typename T> FastAllocator<T>::FastAllocator(void * memory_start, size_t size_in_objects) : memory_start_(memory_start), size_in_objects_(size_in_objects) {
		first_element_ = reinterpret_cast<Element *>(memory_start_);
		initialize();
	}

	template <typename T> T* FastAllocator<T>::allocate() {
		if (first_element_ == nullptr) {
			throw std::bad_alloc();
		}

		T *ret = reinterpret_cast<T *>(first_element_);
		first_element_ = first_element_->next_;

		return ret;
	}

	template <typename T> void FastAllocator<T>::deallocate(T* object) {
		Element *start = reinterpret_cast<Element *>(memory_start_);
		Element *element = reinterpret_cast<Element *>(object);

		size_t diff = element - start;

		if (diff < 0 || diff > size_in_objects_) {
			std::string message;
			message += "FastAllocator<";
			message += typeid(T).name();
			message += ">::deallocate - parameter \"object\" of type ";
			message += typeid(T*).name();
			message += " is invalid";
			throw std::invalid_argument(message);
		}

		element->next_ = first_element_;
		first_element_ = element;
	}

	template <typename T> void FastAllocator<T>::initialize() {
		Element *start = first_element_;
		Element *end = start + size_in_objects_;
		for (Element *curr = start; curr != end; ++curr) {
			curr->next_ = curr + 1;
		}

		Element *last = end - 1;
		last->next_ = nullptr;
	}
}

#endif
