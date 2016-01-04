/**
* \file Cell.cpp
*/

#include "Cell.hpp"
#include "cons.hpp"
#include <cmath>

Cell* const nil = new NilCell(); // sentinel node
vector<string> op; // store primitives
vector<string>::iterator find_op;
CellDef fstack; // fstack[0] for global variables defined, fstack[1] for user-defined functions to be stored.
CellMap::iterator find_key;

using namespace std;

void Cell::initialize() const
{
	fstack.resize(2); // initialize 2 empty maps for fstack
	op.resize(21); // global primitives reference table
	op[0] = "if"; op[1] = "+"; op[2] = "-"; op[3] = "*"; op[4] = "/";
	op[5] = "ceiling"; op[6] = "floor"; op[7] = "quote";
	op[8] = "cons"; op[9] = "car"; op[10] = "cdr"; op[11] = "nullp";
	op[12] = "define"; op[13] = "<"; op[14] = "not"; op[15] = "print"; op[16] = "eval";
	op[17] = "lambda", op[18] = "apply";
	op[19] = "null?", op[20] = "list?";
}

void ConsCell::print(std::ostream& os) const
{
	Cell* cdr = cdr_m;
	Cell* car = cdr->get_car();
	os << "(";
	car_m->print();
	while (cdr != nil) {
		bool isCons = cdr->is_cons();
		os << " ";
		if (isCons){
			if (car == nil) {
				os << "()";
				break;
			} else {
				cdr->get_car()->print();
				cdr = cdr->get_cdr();
			}
		} else {
			cdr->print();
			break;
		}
	}
	os << ")";
}

bool ConsCell::check_number(CellVec v) const
{
	int n = v.size();
	for (int j = 0; j < n; ++j) {
		eval_cons(v, j);
		if (!v[j]->is_int() && !v[j]->is_double())
			return false;
	} return true;
}

Cell* ConsCell::make_number_cell(double d, bool any_double) const {
	if (any_double)
		return make_double(d);
	else
		return make_int(d);
}

void ConsCell::make_cell_vector(CellVec& v) const
{
	Cell* car = car_m;
	v.push_back(car);
	if (cdr_m != nil)
		cdr_m->make_cell_vector(v);
}

Cell* ConsCell::ceval()
{
	if (car_m->ceval()->is_procedure())
		return operate();
	else if (car_m->is_cons()) {
		//if (!car_m->ceval()->is_procedure())
		//	throw RuntimeError("Cannot apply a value that is not a procedure\n     : At Cell* ConsCell::ceval()");
		return car_m->ceval();
	}
	else if (car_m->is_nil())
		throw RuntimeError("Empty list\n     : At Cell* ConsCell::ceval()");
	else if (car_m->is_symbol()) {
 		string s = car_m->get_symbol();
		find_op = locate(op.begin(), op.end(), s);
		if (find_op != op.end())
			return operate();
 		find_key = fstack[0].find(s);
		if (find_key != fstack[0].end())			
			return operate();
		find_key = fstack[1].find(s);
		if (find_key != fstack[1].end())
			return operate();
		else if (fstack.size() > 2) {
			for (int i = fstack.size() - 1; i > 1; --i) {
				find_key = fstack[i].find(s);
				if (find_key != fstack[i].end() && find_key->second->is_procedure())
					return operate();
			}
		}
		//throw RuntimeError("Attempt to reference an unbound variable \"" + s + "\"" + "\n     : At Cell* ConsCell::ceval()");
	}
	return this;
}

void ConsCell::eval_cons(CellVec& v, int i) const
{
	if (v[i]->is_cons()) 
		v[i] = v[i]->ceval();
}

void ConsCell::replace_variable(CellVec& v, string s) const
{
	for (int i = 0; i < v.size(); ++i) {
		if (v[i]->is_symbol()) {
			string s2 = v[i]->get_symbol();
			for (int j = fstack.size() - 1; j >= 0; --j) {
				find_key = fstack[j].find(s2);
				if (find_key != fstack[j].end())
				{ v[i] = find_key->second->ceval(); break; }
				else continue;
			} // inner for			
			find_op = locate(op.begin(), op.end(), s);
			if (find_op != op.end() && fstack.size() < 2) {
				find_key = fstack[0].find(s2);
				if (find_key == fstack[0].end())
					throw RuntimeError("Attempt to reference an unbound variable \"" + s2 + "\"" + "\n     : At Cell* ConsCell::replace_variable()");
				else {
					if (s == "eval")  v[i] = find_key->second;
					else  v[i] = find_key->second->ceval();
				}
			} //if
		} // outter if
	} // outter for
	// replace all defined variables stored in the stack frame, with the value, for the necessary operators
}

Cell* ConsCell::operate() const
{
	string s = ""; // null string
	CellVec v; // create CellVec class instance
	cdr_m->make_cell_vector(v);
	if (!car_m->ceval()->is_procedure())
		s = car_m->ceval()->get_symbol();
	else { replace_variable(v, s); return car_m->ceval()->apply(v, s); }// when anonymous procedure
	find_op = locate(op.begin(), op.end(), s);
	find_key = fstack[1].find(s);
	if ((find_op != op.end() || find_key != fstack[1].end()) && 
			s != "car" && s != "cdr" && s != "quote" && s != "eval" && s != "define" && s != "lambda")
			replace_variable(v, s);	
	find_key = fstack[1].find(s);
	if (find_key != fstack[1].end()) { return find_key->second->ceval()->apply(v, s); }
	else if (fstack.size() > 2) {
		for (int i = fstack.size() - 1; i > 1; --i) {
			find_key = fstack[i].find(s);
			if (find_key != fstack[i].end() && find_key->second->is_procedure())
				return find_key->second->ceval()->apply(v, s);
		}
	} // check if replaced variable is a defined function or primitive
	int n = v.size();
	for (int i = 0; i < n; ++i) {
		if ((v[i]->is_nil() || v[i]->is_cons()) && s != "quote" && s != "lambda") {
			if (v[i]->is_cons() && !v[i]->is_nil()) {
				if (v[i]->get_car()->is_symbol()) {
					string str = v[i]->get_car()->get_symbol();
					find_op = locate(op.begin(), op.end(), str);
					if (find_op == op.end() && fstack.size() < 1)
						throw RuntimeError("Cannot evaluate an invalid list\n     : At Cell* ConsCell::operate()");
				}
				//else if (!v[i]->get_car()->is_symbol() && !v[i]->get_car()->is_cons() && !v[i]->get_car()->is_procedure())
				//	throw RuntimeError("Not a procedure\n     : At Cell* ConsCell::operate()");
			}	/*else
				throw RuntimeError("Cannot evaluate a null expression\n     : At Cell* ConsCell::operate()");*/
		} // outter if
	} // for
	if (n == 0) {
		if (s == "+")  return make_number_cell(0, false);
		else if (s == "*" || s == "<")  return make_number_cell(1, false);
		else  throw RuntimeError("The current operator \"" + s + "\"" + " has been called with 0 arguments\n     : At Cell* ConsCell::operate()");
	} // identity value 
	if (n != 1) {
		if (s == "quote" || s == "print" || s == "eval" || s == "ceiling" || s == "floor" || s == "nullp" || s == "not")
			throw RuntimeError("The current operator \"" + s + "\" only accepts one operand" + "\n     : At Cell* ConsCell::operate()");
	}
	if (n != 2) {
		if (s == "cons" || s == "define" || s == "apply")
			throw RuntimeError("The current operator \"" + s + "\" only accepts two operands" + "\n     : At Cell* ConsCell::operate()");
	}

	if (s == "if") {
		if (v[0]->ceval()->is_nil())
			throw RuntimeError("First operand cannot be null\n     : At Cell* ConsCell::operate()");
		else if (n > 3)
			throw RuntimeError("Operator \"if\" requires at most 3 operands\n     : At Cell* ConsCell::operate()");
		else if (v[0]->ceval()->is_zero()) {
			if (n < 3)
				throw RuntimeError("Operator \"if\" requires exactly 3 operands for this operation\n     : At Cell* ConsCell::operate()");
			return v[2]->ceval();
		}
		else if (n < 2)
			throw RuntimeError("Operator \"if\" requires at least 2 operands\n     : At Cell* ConsCell::operate()");
		return v[1]->ceval();
	}
	else if (s == "+" || s == "-" || s == "*" || s == "/") {
		replace_variable(v, s);
		for (int i = 0; i < n; ++i)
			eval_cons(v, i);
		if (!check_number(v))
			throw RuntimeError("Attempt to reference an unbound variable for \"" + s + "\"\n     : At Cell* ConsCell::operate()");
		if (v[0]->ceval()->is_cons())
			throw RuntimeError("Cannot \"" + s + "\" a list");
		double d = v[0]->ceval()->get_value();
		int i = 0;
		bool any_double = (v[0]->ceval()->is_double()); // flag any_double if there is a double input.				
		if (!any_double)
			i = d;
		if (n == 1 && s == "/") {
			d = 1 / d;
			if (i != 0)
				i = 1 / i;
		}
		for (int k = 1; k < n; ++k) {
			if (!any_double && v[k]->ceval()->is_double())
				any_double = true;
			double vk = v[k]->ceval()->get_value();
			if (s == "+")
				d += vk;
			else if (s == "*")
				d *= vk;
			else if (s == "-")
				d -= vk;
			else if (s == "/") {
				if (v[0]->ceval()->is_zero() || v[k]->ceval()->is_zero())
					throw RuntimeError("Division by zero\n     : At Cell* ConsCell::operate()");
				if (i != 0) {
					i /= vk;
					return make_number_cell(i, any_double);
				}
				d /= vk;
				any_double = true;
			}
		} // for
		if (s == "/" && d == 0)
			throw RuntimeError("Division by zero\n     : At Cell* ConsCell::operate()");
		if (s == "-" && n == 1)
			d *= -1;
		return make_number_cell(d, any_double);
	} // else if
	else if (s == "ceiling" || s == "floor") {
		replace_variable(v, s);
		if (!v[0]->ceval()->is_double())
			throw RuntimeError("The current operator \"" + s + "\" accepts only double operands" + "\n     : At Cell* ConsCell::operate()");
		double d = v[0]->ceval()->get_value();
		if (s == "ceiling")	d = ceil(d);
		else d = floor(d);
		return make_number_cell(d, false);
	}
	else if (s == "quote")  return v[0];
	else if (s == "cons") {
		//if (v[0]->is_cons() && !(v[0]->get_car()->is_symbol()))
		//	throw RuntimeError("Illfully formed list for \"" + s + "\"\n     : At Cell* ConsCell::operate()");
		//if (v[0] == nil || (v[1] != nil && !v[1]->is_cons()))
		//	throw RuntimeError("The current operator \"cons\" requires \"nil\" only as the end of the list\n     : At Cell* ConsCell::operate()");
		Cell* car = v[0]->ceval();
		Cell* cdr = v[1]->ceval();
		return cons(car, cdr->ceval());
	}
	else if (s == "car") {
		if (v[0]->is_cons() && !(v[0]->get_car()->is_symbol()))
			throw RuntimeError("Illfully formed list for \"" + s + "\"\n     : At Cell* ConsCell::operate()");
		for (int i = 0; i < n; ++i)
			eval_cons(v, i);
		replace_variable(v, s);
		return v[0]->get_car();
	}
	else if (s == "cdr") {
		if (v[0]->is_cons() && !(v[0]->get_car()->is_symbol()))
			throw RuntimeError("Illfully formed list for \"" + s + "\"\n     : At Cell* ConsCell::operate()");
		for (int i = 0; i < n; ++i)
			eval_cons(v, i);
		replace_variable(v, s);
		return v[0]->get_cdr();
	}
	else if (s == "nullp" || s == "null?") {
		if (v[0]->ceval() == nil)	return make_number_cell(1, false);
		else return make_number_cell(0, false);
	}
	else if (s == "list?") {
		if (listp(v[0]->ceval()))	return make_number_cell(1, false);
		else return make_number_cell(0, false);
	}
	else if (s == "<") {
		bool all_increase = true;
		for (int i = 0; i < n; ++i) {
			eval_cons(v, i);
			if (!v[i]->is_int() && !v[i]->is_double())
				throw RuntimeError("Attempt to reference an unbound variable for \"" + s + "\"\n     : At Cell* ConsCell::operate()");
			if (i+1 < n) {
				if (v[i]->ceval()->get_value() >= v[i+1]->ceval()->get_value()) {
					all_increase = false;
				}
			} // if
		} // for
		if (!all_increase) return make_number_cell(0, false);
		if (n == 1) return make_number_cell(1, false);
		return make_number_cell(1, false);
	}
	else if (s == "not") {
		if (v[0]->ceval()->is_nil())  return make_number_cell(0, false);
		if (v[0]->ceval()->is_cons()) return make_number_cell(0, false);
		if (v[0]->ceval()->get_value() != 0) return make_number_cell(0, false);
		else return make_number_cell(1, false);
	}
	else if (s == "print") {
		v[0]->ceval()->print();
		cout << endl;
		return nil;
	}
	else if (s == "eval") {
		if (v[0]->is_symbol())
			replace_variable(v, s);
		for (int i = 0; i < n; ++i)
			eval_cons(v, i);
		replace_variable(v, s);
		return v[0]->ceval();
	}
	else if (s == "define") {
		if (v[0]->is_symbol()) {
			string key = v[0]->get_symbol();
			find_op = locate(op.begin(), op.end(), key);
			find_key = fstack[0].find(key);
			if (find_op != op.end())
				throw RuntimeError("Cannot define an existing primitive operator \"" + key + "\"" + "\n     : At Cell* ConsCell::operate()");
			if (v[1]->is_symbol()) {
				string str = v[1]->get_symbol();
				find_key = fstack[0].find(str);
				if (find_key == fstack[0].end())
					throw RuntimeError("Attempt to reference an unbound variable \"" + str + "\"" + "\n     : At Cell* ConsCell::operate()");
				fstack[0][key] = find_key->second;
			}	else if (v[1]->is_cons()) {
				if (v[1]->ceval()->is_procedure()) {
					fstack[1][key] = v[1]; // populate fstack[1] only with functions
				}	else
					fstack[0][key] = v[1]; // populate fstack[0] only with variables
			}	else
				fstack[0][key] = v[1];
		} else if (v[0]->is_cons())
			throw RuntimeError("Attempt to define a list\n     : At Cell* ConsCell::operate()");
		else
			throw RuntimeError("Cannot define a number\n     : At Cell* ConsCell::operate()");
		return nil;
	}
	else if (s == "lambda")  { return lambda(v[0], cdr_m->get_cdr()); }
	else if (s == "apply") {
		if (!v[0]->is_symbol() && !v[0]->ceval()->is_procedure())
			throw RuntimeError("Input argument is not a list1\n     : At Cell* ConsCell::operate()");
		else if (v[0]->ceval()->is_procedure()) {
			if (!v[1]->is_cons())
				throw RuntimeError("Input argument is not a list2\n     : At Cell* ConsCell::operate()");
			else {
				CellVec list;
				v[1]->ceval()->make_cell_vector(list);
				return v[0]->ceval()->apply(list,"");
			}
		}
		else {
			string s2 = v[0]->get_symbol();
			find_key = fstack[1].find(s2);
			if (!v[1]->is_cons())
				throw RuntimeError("Input argument is not a list3\n     : At Cell* ConsCell::operate()");
			else if (find_key != fstack[1].end()){
				CellVec list;
				v[1]->ceval()->make_cell_vector(list);
				find_key = fstack[1].find(s2);
				return find_key->second->ceval()->apply(list, s2);
			}
		}
	}
	else { return car_m; }
}

Cell* ProcedureCell::pop_fstack(Cell* const c) const
{
	if (fstack.size() > 2)
		fstack.pop_back();
	return c;
}

Cell* ProcedureCell::apply(CellVec& args, string s) const
{
	// map the local variables on global stack frame.
	// evaluate function.
	find_key = fstack[1].find(s);
	Cell* cur;
	if (find_key != fstack[1].end())
		cur = find_key->second->ceval(); // lambda expressions of the defined key
	else
		cur = lambda(formals_m, body_m); // anonymous functions
	Cell* body = cur->get_body();
	CellVec body_v; // body vector
	Cell* formals = cur->get_formals();
	CellVec formals_v; // formals vector
	CellMap locals; // local variables definition map

	while (!body->is_nil()) {
		body_v.push_back(car(body));
		body = body->get_cdr();
	} // populate body_v
	while (!formals->is_nil()) {
		if (formals->get_car()->is_symbol()) {
			formals_v.push_back(formals->get_car());
			formals = formals->get_cdr();
		} 
		else
			throw RuntimeError("Arguement is not symbol\n     : At Cell* ProcedureCell::apply()");
	} // populate formals_v
	if (args.size() == formals_v.size()) {
		for (int i = 0; i < args.size(); ++i)
			locals[formals_v[i]->get_symbol()] = args[i]->ceval();
		fstack.push_back(locals);
	} // populate CellMap locals and push it into the stack frame
	else
		throw RuntimeError("Arguement requirement is not matched\n     : At Cell* ProcedureCell::apply()");
	if (body_v.size() > 1) {
		for (int i = 0; i < body_v.size(); ++i) {
			if (i == body_v.size()-1)
				return pop_fstack(body_v[i]->ceval());
			body_v[i]->ceval();
		}
	}
	return pop_fstack(body_v[0]->ceval());
}