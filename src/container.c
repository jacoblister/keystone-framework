/* Note, this file is not built, it exists purely for documentation */

/*! \defgroup ADT Abstract Data Types
 * Abstract Data Containers.
 */
 
/*! \addtogroup ADT 
 *  @{
 */
/*! \brief Linked Lists
 * 
 */
template <class type>
class LIST {
 public:
};
/*! @} */

/*! \addtogroup ADT 
 *  @{
 */
/*! \brief Dynamic Arrays
 *
 * Quote Wikipedia - <i>
 * "In computer science, a dynamic array, growable array, resizable array, dynamic table, or 
 * array list is an array data structure that can be resized and allows elements to be added
 * or removed. It is supplied with standard libraries in many modern mainstream programming languages.
 * dynamic array is not the same thing as a dynamically-allocated array, which is a fixed-size array 
 * whose size is fixed when the array is allocated."
 * </i>
 *
 * Array indexes are zero based.  It is legal to provide an index the same count as the array for insert operations, even
 * though such an item does not exist prior to the insert.
 * Negative index values reference items starting from the end of the array, -1 being the last element in the array
 * <table border="1" bordercolour="lightgrey" frame="box" rules="all" cellspacing="0">
 * <tr align="center"><td>0</td><td>1</td><td>2</td><td>3</td><td>4</td><td>5</td></tr>
 * <tr align="center"><td><b>I</b></td><td><b>N</b></td><td><b>D</b></td><td><b>E</b></td><td><b>X</b></td></tr>
 * <tr align="center"><td>-5</td><td>-4</td><td>-3</td><td>-2</td><td>-1</td></tr>
 * </table>
 * \invariant
 * \code ARRAY(this).count() <= ARRAY(this).capacity() \endcode
 * \note
 * Declaration of arrays of a specified type needs to be made before arrays of specified type can be declared themselves
 * \code ARRAY:typedef<int>; \endcode  
 */
template <class type, int capacity>
class ARRAY {
 public:
   ARRAY(); 
   void new(void);
   void new(void *data, int capacity, int count);
   void delete(void);
   
   int capacity(void);
   bool capacity_dynamic(void);   
   int capacity_set(int);
   int count(void);      
   int item_size(void);      
   type item(int index);   
   bool item_find_first(type item, int *index_result);
   bool item_find_last(type item, int *index_result);   
   void item_insert(type item, int index);
   void item_append(type item);
   void item_prepend(type item);
   void n_item_insert(type item, int count, int index);      
   void n_item_append(type *item, int count);
   void n_item_prepend(type *item, int count);
   void array_insert(ARRAY<type> *src, int index);
   void array_append(ARRAY<type> *src);
   void array_prepend(ARRAY<type> *src);
   
   type remove_item(int index);
   *ARRAY<type> remove_slice(ARRAY<type> *dest, int index_begin, int index_end);
   
   *ARRAY<type> copy(ARRAY<type> *dest);      
   *ARRAY<type> copy_slice(ARRAY<type> *dest, int index_begin, int index_end);      
};
 /*! @} */
 
template <class type, int capacity> ARRAY<type, capacity>::ARRAY() {
/*! \brief Declate a dynamic array.
 *  \param type datatype of dynamic array
 *  \param capacity <i>(optional)</i> maximum capacity of array:
 *         - If specifed, memory is allocated statically at time of declaration, and capacity cannot be changed. 
 *         - If not specifed, array memory is dynamically allocated at run time, and capacity can be changed
 *           as needed.  
 *  
 *  Array of integers with maximum capacity of 100 items
 *  \code ARRAY<int, 100> array_int; \endcode
 *  Array of integers with dynamic maximum capacity
 *  \code ARRAY<int> array_int; \endcode
 *
 *  Arrays may be statically initialized in some cases.  In this case, the use of new() and delete() methods is
 *  optional.  When arrays are not statically initialized, the use of new() and delete() methods is manditory.
 *  It is not posible to statically initialize an array with a dynamic maximum capacity
 *
 *  An array of integers with maximum capacity of 100 items, four initial items with values 2, 4, 8, 16
 *  \code ARRAY<int, 100> array_int = [2,4,8,16]; \endcode
 *  An array of integers with maximum capacity of 100 items, initialized as empty
 *  \code ARRAY<int, 100> array_int = NULL; \endcode
 */
}

template <class type, int capacity> ARRAY<type, capacity>::new() {
/*! \brief Initialize a dynamic array for use 
 *  Declare an Array of integers with dynamic maximum capacity, and initialize ready for use
 *  \code 
 * ARRAY<int> array_int; 
 *
 * ARRAY(&array_int).new(); 
 *  .. make use of array ..
 * ARRAY(&array_int).delete();
 * \endcode
 */  
}

template <class type, int capacity> ARRAY<type, capacity>::new(void *data, int capacity, int count) {
/*! \brief Initialize a dynamic array for use with a preallocated buffer
 *  \param data preallocated buffer to use as array storage
 *  \param capacity capacity of buffer (number of items)
 *  \param count number of items initially used in preallocated buffer
 *
 *  Declare an Array of integers, and initialize with a preallocated buffer ready for use 
 *  \code 
 * int buffer[1000] = {1, 2, 3};
 * ARRAY<int> array_int; 
 *
 * ARRAY(&array_int).new(buffer, sizeof(buffer) / sizeof(buffer[0]), 3); 
 *  .. make use of array ..
 * ARRAY(&array_int).delete();
 * \endcode
 * \note
 * Memory management of the buffer is entirely the callers responsibility in this configuration
 * calling delete() on the array at the end of it's use is optional, but if this is called, no
 * attempt to free the preallocated buffer will be made
 */  
}

template <class type, int capacity> ARRAY<type, capacity>::delete() {
/*! \brief Release a dynamic array after use, counterpart to new() */  
}

template <class type, int capacity> int ARRAY<type, capacity>::capacity(void) {
/*! \brief Current maximum capacity of a dymanic array 
 *  \return current maximum capacity
 */
}
template <class type, int capacity> int ARRAY<type, capacity>::capacity_dynamic(void) {
/*! \brief does array have dynamic capacity?
 *  \return TRUE is dynamic capacity, FALSE otherwise
 */
}
template <class type, int capacity> int ARRAY<type, capacity>::capacity_set(int capacity) {
/*! \brief Reset capacity of dynamic array
 *  \pre \code ARRAY(this).capacity_dynamic() == TRUE \endcode
 *  \pre \code capacity >= ARRAY(this).count() \endcode 
 *  \return old capacity
 */
}

template <class type, int capacity> int ARRAY<type, capacity>::count(void) {
/*! \brief Current number of items in array
 *  \return current item count
 */
}

template <class type, int capacity> int ARRAY<type, capacity>::item_size(void) {
/*! \brief Size of single item in array
 *  \return size in bytes of a single item in the array
 */
}

template <class type, int capacity> type *ARRAY<type, capacity>::item(int index) {
/*! \brief Return single item in array 
 *  \pre \code index >= -ARRAY(this).count() \endcode
 *  \pre \code index < ARRAY(this).count() \endcode
 *  \param index position of item in array to retrieve
 *  \return reference to item */
}

template <class type, int capacity> void ARRAY<type, capacity>::item_append(type item) {
/*! \brief Add item to the end of the array.
 *  Has the same effect as \code ARRAY(this).item_insert(item, ARRAY(this).count()) \endcode
 */
}

template <class type, int capacity> void ARRAY<type, capacity>::item_prepend(type item) {
/*! \brief Add item to the begining of the array.
 *  Has the same effect as \code ARRAY(this).item_insert(item, 0) \endcode
 */
}

   