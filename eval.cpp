/**
* \file eval.cpp
*
*/

#include "eval.hpp"
#include <cmath>

Cell* eval(Cell* const c)
{
	initialize(c);
	if (c == nil)
		throw RuntimeError("Empty list\n     : At Cell* eval()");
	string s;
	if (!listp(c) && symbolp(c) && fstack.empty())
		throw RuntimeError("Attempt to reference an unbound variable \"" + get_symbol(c) + "\"" + "\n     : At Cell* eval()");
	else if (!listp(c) && !fstack.empty() && symbolp(c)) {
		s = get_symbol(c);
		CellMap::iterator find_key;
		if (fstack.size() > 1) {
			find_key = fstack[1].find(s);
			if (find_key != fstack[1].end())
				return ceval(find_key->second);
		}
		find_key = fstack[0].find(s);
		if (find_key == fstack[0].end())
			throw RuntimeError("Attempt to reference an unbound variable \"" + s + "\"" + "\n     : At Cell* eval()");
		return ceval(find_key->second);
	}
	if (!listp(c) && !symbolp(c))
		return c;
	else if (listp(c) && !symbolp(car(c)) && !listp(car(c)))
		throw RuntimeError("Invalid operator\n     : At Cell* eval()");
	else if (listp(car(c))) {
		if (nullp(car(c)))
			throw RuntimeError("Cannot evaluate a null expression\n     : At Cell* eval()");
		return ceval(c); // pass it to ceval if it's a double list
	}
	s = get_symbol(car(c));
	vector<string>::iterator find_op = locate(op.begin(), op.end(), s);
	if (find_op != op.end())
		return ceval(c);
	else if (fstack.size() > 1) {
		CellMap::iterator find_key = fstack[1].find(s);
		if (find_key != fstack[1].end())
			return ceval(c);// return apply(ceval(find_key->second), cdr(c));
	}
	throw RuntimeError("Invalid operator \"" + s + "\"\n     : At Cell* eval()");
}