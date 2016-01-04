/**
* \file cons.hpp
*
* Encapsulates an abstract interface layer for a cons list ADT,
* without using member functions. Makes no assumptions about what
* kind of concrete type Cell will be defined to be.
*/

#ifndef CONS_HPP
#define CONS_HPP

#include "Cell.hpp"
#include <string>
#include <iostream>
#include <stdexcept>

/**
* \brief The null pointer value.
*/
extern Cell* const nil;
extern vector<string> op;
extern vector<CellMap> fstack;

/**
* \brief Logic Error exception handler.
* \param msg The relevant detailed error message.
*/
struct LogicError : public logic_error
{
	LogicError(const string& msg = "") : logic_error(msg) {}
};

/**
* \brief Runtime Error exception handler.
* \param msg The relevant detailed error message.
*/
struct RuntimeError : public runtime_error
{
	RuntimeError(const string& msg = "") : runtime_error(msg) {}
};

/**
* \brief Function template for locating.
* \param Iterator begin and end, and T type value.
*/
template<class IteratorT, class T>
inline IteratorT locate(IteratorT begin, IteratorT end, const T& value)
{
	while (begin != end && *begin != value) {
		++begin;
	}
	return begin;
}

/**
* \brief Evaluator.
* \return Evaluated value of c.
*/
inline Cell* ceval(Cell* const c)
{
	return c->ceval();
}

///**
//* \brief evaluator.
//* \return evaluated value of c.
//*/
//inline Cell* apply(Cell* const c)
//{
//	return c->apply();
//}

/**
* \brief Initialize operator vector.
*/
inline void initialize(Cell* const c)
{
	c->initialize();
}

/**
* \brief Make an int cell.
* \param i The initial int value to be stored in the new cell.
*/
inline Cell* make_int(const int i)
{
	return new IntCell(i);
}

/**
* \brief Make a double cell.
* \param d The initial double value to be stored in the new cell.
*/
inline Cell* make_double(const double d)
{
	return new DoubleCell(d);
}

/**
* \brief Make a symbol cell.
* \param s The initial symbol name to be stored in the new cell.
*/
inline Cell* make_symbol(const char* const s)
{
	return new SymbolCell(s);
}

/**
* \brief Make a conspair cell.
* \param my_car The initial car pointer to be stored in the new cell.
* \param my_cdr The initial cdr pointer to be stored in the new cell.
*/
inline Cell* cons(Cell* const my_car, Cell* const my_cdr)
{
	return new ConsCell(my_car, my_cdr);
}

/**
* \brief Make a procedure cell.
* \param my_formals A list of the procedure's formal parameter names.
* \param my_body The body (an expression) of the procedure.
*/
inline Cell* lambda(Cell* const my_formals, Cell* const my_body)
{
	return new ProcedureCell(my_formals, my_body);
}

/**
* \brief Check if c points to an empty list, i.e., is a null pointer.
* \return True iff c points to an empty list, i.e., is a null pointer.
*/
inline bool nullp(Cell* const c)
{
	return c->is_nil();
}

/**
* \brief Check if c points to a list (i.e., nil or a cons cell).
* \return True iff c points to a list (i.e., nil or a cons cell).
*/
inline bool listp(Cell* const c)
{
	return nullp(c) || c->is_cons();
}

/**
* \brief Check if c is a procedure cell.
* \return True iff c is a procedure cell.
*/
inline bool procedurep(Cell* const c)
{
	return !nullp(c) && c->is_procedure();
}

/**
* \brief Check if c points to an int cell.
* \return True iff c points to an int cell.
*/
inline bool intp(Cell* const c)
{
	return !nullp(c) && c->is_int();
}

/**
* \brief Check if c points to a double cell.
* \return True iff c points to a double cell.
*/
inline bool doublep(Cell* const c)
{
	return !nullp(c) && c->is_double();
}

/**
* \brief Check if c points to a symbol cell.
* \return True iff c points to a symbol cell.
*/
inline bool symbolp(Cell* const c)
{
	return !nullp(c) && c->is_symbol();
}

/**
* \brief Accessor (error if c is not an int cell).
* \return The value in the int cell pointed to by c.
*/
inline int get_int(Cell* const c)
{
	return c->get_value();
}

/**
* \brief Accessor (error if c is not a double cell).
* \return The value in the double cell pointed to by c.
*/
inline double get_double(Cell* const c)
{
	return c->get_value();
}

/**
* \brief Retrieve the symbol name as a string (error if c is not a
* symbol cell).
* \return The symbol name in the symbol cell pointed to by c.
*/
inline std::string get_symbol(Cell* const c)
{
	return c->get_symbol();
}

/**
* \brief Accessor (error if c is not a cons cell).
* \return The car pointer in the cons cell pointed to by c.
*/
inline Cell* car(Cell* const c)
{
	return c->get_car();
}

/**
* \brief Accessor (error if c is not a string cell).
* \return The cdr pointer in the cons cell pointed to by c.
*/
inline Cell* cdr(Cell* const c)
{
	return c->get_cdr();
}

/**
* \brief Accessor (error if c is not a procedure cell).
* \return Pointer to the cons list of formal parameters for the function
* pointed to by c.
*/
inline Cell* get_formals(Cell* const c)
{
	return c->get_formals();
}

/**
* \brief Accessor (error if c is not a procedure cell).
* \return Pointer to the cons list containing the expression defining the
* body for the function pointed to by c.
*/
inline Cell* get_body(Cell* const c)
{
	return c->get_body();
}
/**
* \brief Print the subtree rooted at c, in s-expression notation.
* \param os The output stream to print to.
* \param c The root cell of the subtree to be printed.
*/
inline std::ostream& operator<<(std::ostream& os, const Cell& c)
{
	c.print(os);
	return os;
}

#endif // CONS_HPP
