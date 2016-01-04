/**
* \mainpage COMP2012H Programming Assignment 3, Fall 2014
*
* \author **Jamin SHIN**
* \author **20145090**
* \author **jmshinaa**
* \author **LA1**
*
* \date **Oct 17th, 2014**
*
* Instructor: <a href="http://www.cs.ust.hk/~dekai/">Dekai Wu</a>
* Due: 2014.10.24 at 23:00 by CASS
*/

/**
* \file Cell.hpp
*
* Encapsulates the abstract interface for a concrete class-based
* implementation of cells for a cons list data structure.
*/

#ifndef CELL_HPP
#define CELL_HPP

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stack>
#include <vector> // STL vector
#include <map> // STL map

#pragma warning (disable: 4996)

using namespace std;

class Cell; // forward declaration of Cell
extern Cell* const nil;
typedef vector<Cell*> CellVec;
typedef map<string, Cell*> CellMap;
typedef vector<CellMap> CellDef;
extern CellDef fstack; // global stack frame
extern vector<string> op; // global storage for operator

/**
* \class Cell
* \brief The Abstract Base Class of IntCell, DoubleCell, SymbolCell, and ConsCell.
*/
class Cell {
public:
	
	virtual ~Cell() {}

	/**
	* \brief Check if this is an IntCell.
	* \return True iff this is an IntCell.
	*/
	virtual bool is_int() const { return false; }

	/**
	* \brief Check if this is an DoubleCell.
	* \return True iff this is an DoubleCell.
	*/	
	virtual bool is_double() const { return false; }
	
	/**
	* \brief Check if this is a symbol cell.
	* \return True iff this is a symbol cell.
	*/
	virtual bool is_symbol() const { return false; }

	/**
	* \brief Check if this is a ConsCell.
	* \return True iff this is a ConsCell.
	*/
	virtual bool is_cons() const { return false; }

	/**
	* \brief Check if this is a ProcedureCell.
	* \return True iff this is a ProcedureCell.
	*/
	virtual bool is_procedure() const { return false; }

	/**
	* \brief Check zero for int_m and double_m.
	* \return True if int_m == 0 or double_m == 0.0.
	*/
	virtual bool is_zero() const  { return false; }

	/**
	* \brief Check if this is a NilCell.
	* \return True iff this is a NilCell.
	*/
	virtual bool is_nil() { return false; }

	/**
	* \brief Accessor
	* \return The value in this cell.
	*/
	virtual double get_value() const { return 0.0; }

	/**
	* \brief Accessor
	* \return The symbol name in this symbol cell.
	*/
	virtual char* get_symbol() const { return 0; }

	/**
	* \brief Accessor
	* \return First child cell.
	*/
	virtual Cell* get_car() const { return nil; }

	/**
	* \brief Accessor
	* \return Rest child cell.
	*/
	virtual Cell* get_cdr() const { return nil; }

	/**
	* \brief Accessor
	* \return Function arguments.
	*/
	virtual Cell* get_formals() const { return nil; }

	/**
	* \brief Accessor
	* \return Function body.
	*/
	virtual Cell* get_body() const { return nil; }

	/**
	* \brief Recursively scan until symbol operator.
	* \return Return value of operator().
	*/
	virtual Cell* ceval() { return this; }

	/**
	* \brief Applies the procedure.
	* \return Result of procedure.
	*/
	virtual Cell* apply(CellVec& args, string s) const { return nil; }
	
	/**
	* \brief Create a Cell* object vector containing the parsed tree.
	*/
	virtual void make_cell_vector(CellVec& v) const {}

	/**
	* \brief Print the subtree rooted at this cell, in s-expression notation.
	* \param os The output stream to print to.
	*/
	virtual void print(std::ostream& os = std::cout) const {}

	/**
	* \brief Initialize operator vector.
	*/
	void initialize() const;
};

class IntCell : public Cell
{
public:
	IntCell(int i) : int_m(i) {}
	virtual ~IntCell() {}
	virtual bool is_int() const { return true; }
	virtual double get_value() const { return int_m; }
	virtual bool is_zero() const  { return int_m == 0; }
	virtual void print(std::ostream& os = std::cout) const { os << int_m; }

private:
	int int_m;
};

class DoubleCell : public Cell
{
public:
	DoubleCell(double d) : double_m(d) {}
	virtual ~DoubleCell() {}
	virtual bool is_double() const { return true; }
	virtual double get_value() const { return double_m; }
	virtual bool is_zero() const { return double_m == 0.0; }
	virtual void print(std::ostream& os = std::cout) const
	{
		os << double_m;
		if (double_m == (int)double_m)
			os << ".0";
	}

private:
	double double_m;
};

class SymbolCell : public Cell
{
public:
	SymbolCell(const char* s) 
	{
		char* str = new char[std::strlen(s)];
		strcpy(str, s);
		symbol_m = str;
	}
	virtual ~SymbolCell() {}
	virtual bool is_symbol() const { return true; }
	virtual char* get_symbol() const { return symbol_m; }
	virtual void print(std::ostream& os = std::cout) const { os << symbol_m; }

private:
	char* symbol_m;
};

class ConsCell : public Cell
{
public:
	ConsCell(Cell* my_car, Cell* my_cdr) : car_m(my_car), cdr_m(my_cdr) {}
	virtual ~ConsCell() {}
	virtual bool is_cons() const { return true; }
	virtual Cell* get_car() const { return car_m; }
	virtual Cell* get_cdr() const { return cdr_m; }
	virtual void print(std::ostream& os = std::cout) const;
	virtual void make_cell_vector(CellVec& v) const;
	virtual Cell* ceval();

	/**
	* \brief Evalute primitive built-in operations.
	* \return Operated Cell* value.
	*/
	Cell* operate() const;

	/**
	* \brief Replace defined symbols in the map with their definitions.
	*/
	void replace_variable(CellVec& v, string s) const;

	/**
	* \brief Check if int or double for array made by get_cell_array.
	* \return True iff elements are either int or double.
	*/
	bool check_number(CellVec v) const;

	/**
	* \brief Check any_double() flagged.
	* \return new DoubleCell or IntCell, accordingly.
	*/
	Cell* make_number_cell(double d, bool any_double) const;

	/**
	* \brief Evaluate nested lists.
	*/
	void eval_cons(CellVec& v, int i) const;


private:
	Cell* car_m;
	Cell* cdr_m;
};

class ProcedureCell : public Cell
{
public:
	ProcedureCell(Cell* const my_formals, Cell* const my_body) : formals_m(my_formals), body_m(my_body) {}
	virtual bool is_procedure() const { return true; }
	virtual Cell* get_formals() const { return formals_m; }
	virtual Cell* get_body() const { return body_m; }
	virtual Cell* apply(CellVec& args, string s) const;
	virtual void print(std::ostream& os = std::cout) const { os << "#<function>"; }

	Cell* pop_fstack(Cell* const c) const;

private:
	Cell* formals_m;
	Cell* body_m;
};

/**
* \class NilCell
* \brief Sentinel class.
*/
class NilCell : public Cell
{
public:
	virtual ~NilCell() { delete nil; }
	virtual bool is_nil() { return true; }
	virtual void print(std::ostream& os = std::cout) const { os << "()"; }
};

#endif // CELL_HPP
