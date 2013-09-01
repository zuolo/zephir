/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is include which follows the "include" declaration
** in the input file. */
#include <stdio.h>
// 44 "parser.lemon"


#include "json/json.h"

#include "string.h"
#include "parser.h"
#include "scanner.h"
#include "xx.h"

static json_object *xx_ret_literal(int type, xx_parser_token *T, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	switch (type) {
		case XX_T_CONSTANT:
			json_object_object_add(ret, "type", json_object_new_string("constant"));
			break;
		case XX_T_IDENTIFIER:
			json_object_object_add(ret, "type", json_object_new_string("variable"));
			break;
		case XX_T_INTEGER:
			json_object_object_add(ret, "type", json_object_new_string("int"));
			break;
		case XX_T_DOUBLE:
			json_object_object_add(ret, "type", json_object_new_string("double"));
			break;
		case XX_T_NULL:
			json_object_object_add(ret, "type", json_object_new_string("null"));
			break;
		case XX_T_STRING:
			json_object_object_add(ret, "type", json_object_new_string("string"));
			break;
		case XX_T_SCHAR:
			json_object_object_add(ret, "type", json_object_new_string("schar"));
			break;
		default:
			if (type == XX_T_TRUE) {
				json_object_object_add(ret, "type", json_object_new_string("bool"));
				json_object_object_add(ret, "value", json_object_new_string("true"));
			} else {
				if (type == XX_T_FALSE) {
					json_object_object_add(ret, "type", json_object_new_string("bool"));
					json_object_object_add(ret, "value", json_object_new_string("false"));
				} else {
					fprintf(stderr, "literal??\n");
				}
			}
	}

	if (T) {
		json_object_object_add(ret, "value", json_object_new_string(T->token));
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_expr(char *type, json_object *left, json_object *right, json_object *extra, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string(type));

	if (left) {
		json_object_object_add(ret, "left", left);
	}
	if (right) {
		json_object_object_add(ret, "right", right);
	}
	if (extra) {
		json_object_object_add(ret, "extra", extra);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_array_item(json_object *key, json_object *value, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	if (key) {
		json_object_object_add(ret, "key", key);
	}
	json_object_object_add(ret, "value", value);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_namespace(xx_parser_token *T, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("namespace"));
	json_object_object_add(ret, "name", json_object_new_string(T->token));

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_class(xx_parser_token *T, json_object *class_definition, int is_abstract, xx_parser_token *E, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("class"));
	json_object_object_add(ret, "name", json_object_new_string(T->token));
	json_object_object_add(ret, "abtract", json_object_new_int(is_abstract));

	if (E) {
		json_object_object_add(ret, "extends", json_object_new_string(E->token));
	}

	if (class_definition) {
		json_object_object_add(ret, "definition", class_definition);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_class_definition(json_object *properties, json_object *methods, json_object *constants, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	if (properties) {
		json_object_object_add(ret, "properties", properties);
	}
	if (methods) {
		json_object_object_add(ret, "methods", methods);
	}
	if (constants) {
		json_object_object_add(ret, "constants", constants);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_class_property(json_object *visibility, xx_parser_token *T, json_object *default_value, xx_parser_token *D, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "visibility", visibility);
	json_object_object_add(ret, "type", json_object_new_string("property"));
	json_object_object_add(ret, "name", json_object_new_string(T->token));

	if (default_value) {
		json_object_object_add(ret, "default", default_value);
	}

	if (D) {
		json_object_object_add(ret, "docblock", json_object_new_string(D->token));
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_class_const(xx_parser_token *T, json_object *default_value, xx_parser_token *D, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("const"));
	json_object_object_add(ret, "name", json_object_new_string(T->token));
	json_object_object_add(ret, "default", default_value);

	if (D) {
		json_object_object_add(ret, "docblock", json_object_new_string(D->token));
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_class_method(json_object *visibility, xx_parser_token *T, json_object *parameters, json_object *statements, xx_parser_token *D, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "visibility", visibility);
	json_object_object_add(ret, "type", json_object_new_string("method"));
	json_object_object_add(ret, "name", json_object_new_string(T->token));

	if (parameters) {
		json_object_object_add(ret, "parameters", parameters);
	}

	if (statements) {
		json_object_object_add(ret, "statements", statements);
	}

	if (D) {
		json_object_object_add(ret, "docblock", json_object_new_string(D->token));
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_parameter(json_object *type, json_object *cast, xx_parser_token *N, json_object *default_value, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("parameter"));
	json_object_object_add(ret, "name", json_object_new_string(N->token));

	if (type) {
		json_object_object_add(ret, "data-type", type);
	}
	if (cast) {
		json_object_object_add(ret, "cast", cast);
	}
	if (default_value) {
		json_object_object_add(ret, "default", default_value);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_type(int type)
{
	switch (type) {
		case XX_TYPE_INTEGER:
			return json_object_new_string("int");
		case XX_TYPE_UINTEGER:
			return json_object_new_string("uint");
		case XX_TYPE_DOUBLE:
			return json_object_new_string("double");
		case XX_TYPE_BOOL:
			return json_object_new_string("bool");
		case XX_TYPE_STRING:
			return json_object_new_string("string");
		case XX_TYPE_CHAR:
			return json_object_new_string("char");
		case XX_TYPE_ARRAY:
			return json_object_new_string("array");
		case XX_TYPE_VAR:
			return json_object_new_string("variable");
		default:
			fprintf(stderr, "err 1?\n");
	}

}

static json_object *xx_ret_list(json_object *list_left, json_object *right_list)
{
	json_object *ret;
	int i, array_length;

	ret = json_object_new_array();

	if (list_left) {

		if (json_object_get_type(list_left) == json_type_array) {
			array_length = json_object_array_length(list_left);
			for (i = 0; i < array_length; i++) {
				json_object_array_add(ret, json_object_array_get_idx(list_left, i));
			}
			//json_object_put(list_left);
		} else {
			json_object_array_add(ret, list_left);
		}

	}

	json_object_array_add(ret, right_list);

	return ret;
}

static json_object *xx_ret_let_statement(json_object *assignments, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("let"));
	json_object_object_add(ret, "assignments", assignments);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_let_assignment(char *type, json_object *operator, xx_parser_token *V, xx_parser_token *P, json_object *index_expr, json_object *expr, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "assign-type", json_object_new_string(type));
	if (operator) {
		json_object_object_add(ret, "operator", operator);
	}
	json_object_object_add(ret, "variable", json_object_new_string(V->token));
	if (P) {
		json_object_object_add(ret, "property", json_object_new_string(P->token));
	}
	if (index_expr) {
		json_object_object_add(ret, "index-expr", index_expr);
	}
	if (expr) {
		json_object_object_add(ret, "expr", expr);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_if_statement(json_object *expr, json_object *statements, json_object *else_statements, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("if"));
	json_object_object_add(ret, "expr", expr);

	if (statements) {
		json_object_object_add(ret, "statements", statements);
	}

	if (else_statements) {
		json_object_object_add(ret, "else_statements", else_statements);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_switch_statement(json_object *expr, json_object *clauses, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("switch"));
	json_object_object_add(ret, "expr", expr);

	if (clauses) {
		json_object_object_add(ret, "clauses", clauses);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_case_clause(json_object *expr, json_object *statements, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("case"));

	if (expr) {
		json_object_object_add(ret, "expr", expr);
	}

	if (statements) {
		json_object_object_add(ret, "statements", statements);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_while_statement(json_object *expr, json_object *statements, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("while"));
	json_object_object_add(ret, "expr", expr);

	if (statements) {
		json_object_object_add(ret, "statements", statements);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_do_while_statement(json_object *expr, json_object *statements, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("do-while"));
	json_object_object_add(ret, "expr", expr);

	if (statements) {
		json_object_object_add(ret, "statements", statements);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_for_statement(json_object *expr, xx_parser_token *K, xx_parser_token *V, int reverse, json_object *statements, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("for"));
	json_object_object_add(ret, "expr", expr);

	if (K) {
		json_object_object_add(ret, "key", json_object_new_string(K->token));
	}
	if (V) {
		json_object_object_add(ret, "value", json_object_new_string(V->token));
	}

	json_object_object_add(ret, "reverse", json_object_new_int(reverse));

	if (statements) {
		json_object_object_add(ret, "statements", statements);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_loop_statement(json_object *statements, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("loop"));

	if (statements) {
		json_object_object_add(ret, "statements", statements);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_break_statement(xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("break"));

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_continue_statement(xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("continue"));

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_echo_statement(json_object *expressions, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("echo"));
	json_object_object_add(ret, "expressions", expressions);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_return_statement(json_object *expr, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("return"));
	json_object_object_add(ret, "expr", expr);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_mcall_statement(json_object *expr, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("mcall"));
	json_object_object_add(ret, "expr", expr);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_scall_statement(json_object *expr, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("scall"));
	json_object_object_add(ret, "expr", expr);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_unset_statement(json_object *domain, json_object *index, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("unset"));

	json_object_object_add(ret, "domain", domain);
	json_object_object_add(ret, "index", index);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_declare_statement(int type, json_object *variables, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("declare"));

	switch (type) {
		case XX_T_TYPE_INTEGER:
			json_object_object_add(ret, "data-type", json_object_new_string("int"));
			break;
		case XX_T_TYPE_UINTEGER:
			json_object_object_add(ret, "data-type", json_object_new_string("uint"));
			break;
		case XX_T_TYPE_LONG:
			json_object_object_add(ret, "data-type", json_object_new_string("long"));
			break;
		case XX_T_TYPE_ULONG:
			json_object_object_add(ret, "data-type", json_object_new_string("ulong"));
			break;
		case XX_T_TYPE_CHAR:
			json_object_object_add(ret, "data-type", json_object_new_string("char"));
			break;
		case XX_T_TYPE_UCHAR:
			json_object_object_add(ret, "data-type", json_object_new_string("uchar"));
			break;
		case XX_T_TYPE_DOUBLE:
			json_object_object_add(ret, "data-type", json_object_new_string("double"));
			break;
		case XX_T_TYPE_BOOL:
			json_object_object_add(ret, "data-type", json_object_new_string("bool"));
			break;
		case XX_T_TYPE_STRING:
			json_object_object_add(ret, "data-type", json_object_new_string("string"));
			break;
		/*case XX_T_TYPE_ARRAY:
			json_object_object_add(ret, "data-type", json_object_new_string("array"));
			break;*/
		case XX_T_TYPE_VAR:
			json_object_object_add(ret, "data-type", json_object_new_string("variable"));
			break;
		default:
			fprintf(stderr, "err 2?\n");
	}

	json_object_object_add(ret, "variables", variables);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_declare_variable(xx_parser_token *T, json_object *expr, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "variable", json_object_new_string(T->token));
	if (expr) {
		json_object_object_add(ret, "expr", expr);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_new_instance(xx_parser_token *T, json_object *parameters, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("new"));
	json_object_object_add(ret, "class", json_object_new_string(T->token));

	if (parameters) {
		json_object_object_add(ret, "parameters", parameters);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_throw_exception(xx_parser_token *T, json_object *parameters, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("throw"));
	json_object_object_add(ret, "domain", json_object_new_string(T->token));

	if (parameters) {
		json_object_object_add(ret, "parameters", parameters);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_fcall(xx_parser_token *F, json_object *parameters, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("fcall"));
	json_object_object_add(ret, "name", json_object_new_string(F->token));

	if (parameters) {
		json_object_object_add(ret, "parameters", parameters);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_mcall(xx_parser_token *O, xx_parser_token *M, json_object *parameters, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("mcall"));
	json_object_object_add(ret, "variable", json_object_new_string(O->token));
	json_object_object_add(ret, "name", json_object_new_string(M->token));

	if (parameters) {
		json_object_object_add(ret, "parameters", parameters);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_scall(xx_parser_token *O, xx_parser_token *M, json_object *parameters, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("scall"));
	json_object_object_add(ret, "class", json_object_new_string(O->token));
	json_object_object_add(ret, "name", json_object_new_string(M->token));

	if (parameters) {
		json_object_object_add(ret, "parameters", parameters);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_comment(xx_parser_token *T, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("comment"));
	json_object_object_add(ret, "value", json_object_new_string(T->token));

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}


// 765 "parser.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    xx_TOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is xx_TOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.
**    xx_ARG_SDECL     A static variable declaration for the %extra_argument
**    xx_ARG_PDECL     A parameter declaration for the %extra_argument
**    xx_ARG_STORE     Code to store %extra_argument into yypParser
**    xx_ARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 154
#define YYACTIONTYPE unsigned short int
#define xx_TOKENTYPE xx_parser_token*
typedef union {
  xx_TOKENTYPE yy0;
  json_object* yy115;
  int yy307;
} YYMINORTYPE;
#define YYSTACKDEPTH 100
#define xx_ARG_SDECL xx_parser_status *status;
#define xx_ARG_PDECL ,xx_parser_status *status
#define xx_ARG_FETCH xx_parser_status *status = yypParser->status
#define xx_ARG_STORE yypParser->status = status
#define YYNSTATE 491
#define YYNRULE 233
#define YYERRORSYMBOL 91
#define YYERRSYMDT yy307
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* Next are that tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
static YYACTIONTYPE yy_action[] = {
 /*     0 */   344,  302,  168,  169,   25,  300,   12,  349,  471,  404,
 /*    10 */   175,  170,  171,  173,  172,  308,  332,  338,  335,   87,
 /*    20 */   341,   46,   48,   50,   23,   59,  393,  186,  187,   70,
 /*    30 */    74,   79,  212,  491,  207,  219,    3,    4,    5,    6,
 /*    40 */   194,  125,  126,  127,  128,  129,  130,  270,  281,  284,
 /*    50 */   199,  166,  299,  317,  320,  323,  326,  329,  344,    8,
 /*    60 */   177,  176,   27,  137,  200,  349,  193,  404,   93,  170,
 /*    70 */   171,  173,  172,  308,  332,  338,  335,   99,  341,   46,
 /*    80 */    48,   50,  422,   59,  393,  186,  187,   70,   74,   79,
 /*    90 */   212,    9,  404,  219,  159,  166,  178,  163,  174,  125,
 /*   100 */   126,  127,  128,  129,  130,  270,  281,  284,   11,  393,
 /*   110 */   299,  317,  320,  323,  326,  329,  344,  222,   95,   97,
 /*   120 */   379,   63,  377,  349,  125,  126,  127,  128,  129,  130,
 /*   130 */   356,  308,  332,  338,  335,  140,  341,   46,   48,   50,
 /*   140 */   221,   59,  165,  186,  187,   70,   74,   79,  212,  408,
 /*   150 */   188,  219,  492,  220,  269,  725,    1,    2,  490,    4,
 /*   160 */     5,    6,   14,  270,  281,  284,  286,  289,  299,  317,
 /*   170 */   320,  323,  326,  329,  344,  466,  449,  457,   54,    7,
 /*   180 */   437,  349,   10,  433,  167,  214,  477,  349,  185,  308,
 /*   190 */   332,  338,  335,  246,  341,   46,   48,   50,  242,   59,
 /*   200 */    21,  186,  187,   70,   74,   79,  212,   51,  184,  219,
 /*   210 */   211,  434,   17,  449,  457,  201,  463,  437,  186,  187,
 /*   220 */   433,  270,  281,  284,  309,  316,  299,  317,  320,  323,
 /*   230 */   326,  329,  344,   22,  168,  169,  378,  347,   24,  349,
 /*   240 */   345,  225,   47,  170,  171,  173,  172,  308,  332,  338,
 /*   250 */   335,  246,  341,   46,   48,   50,  255,   59,  161,  186,
 /*   260 */   187,   70,   74,   79,  212,  203,  205,  219,  140,  226,
 /*   270 */   227,  228,  229,  230,   62,  238,  186,  187,  190,  270,
 /*   280 */   281,  284,   49,  189,  299,  317,  320,  323,  326,  329,
 /*   290 */   344,  114,  116,  118,   58,  275,   66,  349,  374,  225,
 /*   300 */    52,  271,  276,  186,  187,  308,  332,  338,  335,  246,
 /*   310 */   341,   46,   48,   50,  263,   59,   55,  186,  187,   70,
 /*   320 */    74,   79,  212,  160,   56,  219,  140,  226,  227,  228,
 /*   330 */   229,  230,   61,  251,  186,  187,  183,  270,  281,  284,
 /*   340 */   290,  189,  299,  317,  320,  323,  326,  329,  344,  435,
 /*   350 */    65,   19,  373,  436,  437,  349,  385,  433,  273,  407,
 /*   360 */   396,  400,   67,  308,  332,  338,  335,  275,  341,   46,
 /*   370 */    48,   50,   68,   59,  274,  186,  187,   70,   74,   79,
 /*   380 */   212,  272,  465,  219,  468,  469,  457,  453,  436,  437,
 /*   390 */   293,  296,  433,  311,  454,  270,  281,  284,   71,  140,
 /*   400 */   299,  317,  320,  323,  326,  329,  344,  186,  187,  196,
 /*   410 */    73,  101,  185,  349,  189,  225,  310,   76,  133,  186,
 /*   420 */   187,  308,  332,  338,  335,  103,  341,   46,   48,   50,
 /*   430 */    80,   59,  191,  186,  187,   70,   74,   79,  212,   83,
 /*   440 */   131,  219,  140,  226,  227,  228,  229,  230,  186,  187,
 /*   450 */   186,  187,  208,  270,  281,  284,  185,  189,  299,  317,
 /*   460 */   320,  323,  326,  329,  344,  311,    7,   85,  372,   10,
 /*   470 */   185,  349,   19,  477,  349,   18,  197,  185,  433,  308,
 /*   480 */   332,  338,  335,  105,  341,   46,   48,   50,  319,   59,
 /*   490 */   209,  186,  187,   70,   74,   79,  212,  306,  233,  219,
 /*   500 */   140,   64,  232,  166,  318,  316,  186,  187,  186,  187,
 /*   510 */   305,  270,  281,  284,  391,  189,  299,  317,  320,  323,
 /*   520 */   326,  329,  344,   66,   60,  374,   78,  211,   91,  349,
 /*   530 */   392,  396,  400,  311,  386,  186,  187,  308,  332,  338,
 /*   540 */   335,   90,  341,   46,   48,   50,   92,   59,  162,  163,
 /*   550 */   174,   70,   74,   79,  212,   75,  322,  219,  211,  321,
 /*   560 */   316,   84,  324,  316,  211,  311,  186,  187,  139,  270,
 /*   570 */   281,  284,  186,  187,  299,  317,  320,  323,  326,  329,
 /*   580 */   344,  233,   94,  107,  368,  237,  391,  349,  325,  186,
 /*   590 */   187,  186,  187,  327,  316,  308,  332,  338,  335,  109,
 /*   600 */   341,   46,   48,   50,   96,   59,  428,  186,  187,   70,
 /*   610 */    74,   79,  212,   98,  233,  219,  233,  480,  241,  484,
 /*   620 */   245,  311,  186,  187,  186,  187,  134,  270,  281,  284,
 /*   630 */   138,  111,  299,  317,  320,  323,  326,  329,  344,  186,
 /*   640 */   187,  113,   82,  233,  328,  349,  144,  250,  145,  186,
 /*   650 */   187,  186,  187,  308,  332,  338,  335,  115,  341,   46,
 /*   660 */    48,   50,  146,   59,  164,  186,  187,   70,   74,   79,
 /*   670 */   212,  233,  708,  219,  233,  254,  330,  316,  258,  186,
 /*   680 */   187,  311,  186,  187,  709,  270,  281,  284,  710,  117,
 /*   690 */   299,  317,  320,  323,  326,  329,  344,  186,  187,  119,
 /*   700 */   218,  233,  180,  349,  331,  262,  181,  186,  187,  186,
 /*   710 */   187,  308,  332,  338,  335,  121,  341,   46,   48,   50,
 /*   720 */   195,   59,  202,  186,  187,   70,   74,   79,  212,  370,
 /*   730 */   233,  219,  211,  204,  266,  333,  316,  213,  186,  187,
 /*   740 */   186,  187,  224,  270,  281,  284,  235,  123,  299,  317,
 /*   750 */   320,  323,  326,  329,  344,  186,  187,  311,  355,  135,
 /*   760 */   427,  349,  206,  407,  396,  400,  223,  186,  187,  308,
 /*   770 */   332,  338,  335,  142,  341,   46,   48,   50,  231,   59,
 /*   780 */   334,  186,  187,   70,   74,   79,  212,  148,  243,  219,
 /*   790 */   215,  336,  316,  339,  316,  186,  187,  311,  186,  187,
 /*   800 */   236,  270,  281,  284,  240,  282,  299,  317,  320,  323,
 /*   810 */   326,  329,  344,  186,  187,  352,  362,  359,  248,  349,
 /*   820 */   337,  244,  311,  186,  187,  186,  187,  308,  332,  338,
 /*   830 */   335,  364,  341,   46,   48,   50,  249,   59,  253,  186,
 /*   840 */   187,   70,   74,   79,  212,  340,  459,  219,  418,  342,
 /*   850 */   316,  256,  311,  460,  433,  278,  451,  257,  261,  270,
 /*   860 */   281,  284,  433,  264,  299,  317,  320,  323,  326,  329,
 /*   870 */   344,  265,  280,  285,  367,  343,  287,  349,  288,  291,
 /*   880 */   292,  294,  295,  297,  298,  308,  332,  338,  335,  301,
 /*   890 */   341,   46,   48,   50,  303,   59,  304,  307,  313,   70,
 /*   900 */    74,   79,  212,  312,  315,  219,  346,  314,  348,  357,
 /*   910 */   369,  358,  371,  375,  380,  381,  387,  270,  281,  284,
 /*   920 */   395,  406,  299,  317,  320,  323,  326,  329,  344,  394,
 /*   930 */   397,  398,  384,  399,  401,  349,  403,  402,  405,  554,
 /*   940 */   420,  440,  442,  308,  332,  338,  335,  421,  341,   46,
 /*   950 */    48,   50,  423,   59,  429,  441,  443,   70,   74,   79,
 /*   960 */   212,  446,  445,  219,  447,  448,  455,  456,  461,  462,
 /*   970 */   472,  475,  473,  478,  479,  270,  281,  284,  485,  482,
 /*   980 */   299,  317,  320,  323,  326,  329,  344,  486,  388,  488,
 /*   990 */   383,  388,  388,  349,  388,  388,  388,  388,  388,  388,
 /*  1000 */   388,  308,  332,  338,  335,  388,  341,   46,   48,   50,
 /*  1010 */   388,   59,  388,  388,  388,   70,   74,   79,  212,  388,
 /*  1020 */   388,  219,  388,  388,  388,  388,  388,  388,  388,  388,
 /*  1030 */   388,  388,  388,  270,  281,  284,  388,  388,  299,  317,
 /*  1040 */   320,  323,  326,  329,  344,  388,  388,  388,  388,  388,
 /*  1050 */   388,  349,  388,  388,  388,  388,  388,  388,  388,  308,
 /*  1060 */   332,  338,  335,  388,  341,   46,   48,   50,  388,   59,
 /*  1070 */   388,  388,  388,   70,   74,   79,  212,  388,  388,  219,
 /*  1080 */   388,  388,  388,  388,  388,  388,  388,  388,  388,  388,
 /*  1090 */   388,  270,  281,  284,  388,  388,  299,  317,  320,  323,
 /*  1100 */   326,  329,  344,  388,  388,  388,  390,  388,  388,  349,
 /*  1110 */   388,  388,  388,  388,  388,  388,  388,  308,  332,  338,
 /*  1120 */   335,  388,  341,   46,   48,   50,  388,   59,  388,  388,
 /*  1130 */   388,   70,   74,   79,  212,  388,  388,  219,  388,  388,
 /*  1140 */   388,  388,  388,  388,  388,  388,  388,  388,  388,  270,
 /*  1150 */   281,  284,  388,  388,  299,  317,  320,  323,  326,  329,
 /*  1160 */   344,  388,  388,  388,  424,  388,  388,  349,  388,  388,
 /*  1170 */   388,  388,  388,  388,  388,  308,  332,  338,  335,  388,
 /*  1180 */   341,   46,   48,   50,  388,   59,  388,  388,  388,   70,
 /*  1190 */    74,   79,  212,  388,  388,  219,  388,  388,  388,  388,
 /*  1200 */   388,  388,  388,  388,  388,  388,  388,  270,  281,  284,
 /*  1210 */   388,  388,  299,  317,  320,  323,  326,  329,  344,  388,
 /*  1220 */   388,  388,  426,  388,  388,  349,  388,  388,  388,  388,
 /*  1230 */   388,  388,  388,  308,  332,  338,  335,  388,  341,   46,
 /*  1240 */    48,   50,  388,   59,  388,  388,  388,   70,   74,   79,
 /*  1250 */   212,  388,  388,  219,  388,  388,  388,  388,  388,  388,
 /*  1260 */   388,  388,  388,  388,  388,  270,  281,  284,  388,  388,
 /*  1270 */   299,  317,  320,  323,  326,  329,  344,  388,  388,  388,
 /*  1280 */   430,  388,  388,  349,  388,  388,  388,  388,  388,  388,
 /*  1290 */   388,  308,  332,  338,  335,  388,  341,   46,   48,   50,
 /*  1300 */   388,   59,  388,  388,  388,   70,   74,   79,  212,  388,
 /*  1310 */   388,  219,  388,  388,  388,  388,  388,  388,  388,  388,
 /*  1320 */   388,  388,  388,  270,  281,  284,  388,  388,  299,  317,
 /*  1330 */   320,  323,  326,  329,  344,  388,  388,  388,  432,  388,
 /*  1340 */   388,  349,  388,  388,  388,  388,  388,  388,  388,  308,
 /*  1350 */   332,  338,  335,  388,  341,   46,   48,   50,  388,   59,
 /*  1360 */   388,  388,  388,   70,   74,   79,  212,  388,  388,  219,
 /*  1370 */   388,  409,  410,  412,  411,  413,  388,  388,  344,  388,
 /*  1380 */   388,  270,  281,  284,  388,  349,  299,  317,  320,  323,
 /*  1390 */   326,  329,  388,  308,  332,  338,  335,  388,  341,   46,
 /*  1400 */    48,   50,  510,   59,  415,  417,  388,   70,   74,   79,
 /*  1410 */   212,  414,  416,  219,  388,  388,  388,  388,  388,  388,
 /*  1420 */   388,  388,  388,  388,  388,  270,  281,  284,  388,  388,
 /*  1430 */   299,  317,  320,  323,  326,  329,  388,  388,  388,  388,
 /*  1440 */   388,  388,  388,  388,  388,  388,   89,  143,  147,  141,
 /*  1450 */   179,   86,  388,  388,  136,  388,  388,  388,  388,  175,
 /*  1460 */   388,  388,  388,   45,  149,  388,  124,  388,  388,  125,
 /*  1470 */   126,  127,  128,  129,  130,  388,  388,  388,  388,   28,
 /*  1480 */    29,   30,   31,   32,   33,   34,   35,   36,   37,   38,
 /*  1490 */    39,   40,   41,   42,   43,   44,  157,  388,  388,  388,
 /*  1500 */   388,  388,  158,  388,  150,  151,   45,  277,  279,  177,
 /*  1510 */   176,  388,  152,  153,  154,  155,  156,   26,  170,  171,
 /*  1520 */   173,  172,  350,   29,   30,   31,   32,   33,   34,   35,
 /*  1530 */    36,   37,   38,   39,   40,   41,   42,   43,   44,  388,
 /*  1540 */   388,  388,  388,  388,  388,   45,  388,  388,  388,  388,
 /*  1550 */   277,  279,  388,  388,  388,  388,   53,  388,  388,  388,
 /*  1560 */   388,  350,   29,   30,   31,   32,   33,   34,   35,   36,
 /*  1570 */    37,   38,   39,   40,   41,   42,   43,   44,   88,  102,
 /*  1580 */   106,  108,  110,  112,  104,  100,  114,  116,  118,  277,
 /*  1590 */   279,  388,  388,  388,  388,   45,  388,  388,  388,  388,
 /*  1600 */   388,  388,  388,  388,  388,  388,   57,  388,  388,  388,
 /*  1610 */   388,  350,   29,   30,   31,   32,   33,   34,   35,   36,
 /*  1620 */    37,   38,   39,   40,   41,   42,   43,   44,  388,  388,
 /*  1630 */    45,  388,  388,  388,  388,  388,  388,  388,  388,  277,
 /*  1640 */   279,   69,  388,  388,  388,  388,  350,   29,   30,   31,
 /*  1650 */    32,   33,   34,   35,   36,   37,   38,   39,   40,   41,
 /*  1660 */    42,   43,   44,  388,  388,   45,  388,  409,  410,  412,
 /*  1670 */   411,  413,  388,  388,  277,  279,   72,  388,  388,  388,
 /*  1680 */   388,  350,   29,   30,   31,   32,   33,   34,   35,   36,
 /*  1690 */    37,   38,   39,   40,   41,   42,   43,   44,   15,  388,
 /*  1700 */   415,  450,  388,  444,  388,   45,  388,  414,  416,  277,
 /*  1710 */   279,  388,  388,  388,  388,  388,   77,  388,  388,  388,
 /*  1720 */   388,  350,   29,   30,   31,   32,   33,   34,   35,   36,
 /*  1730 */    37,   38,   39,   40,   41,   42,   43,   44,  388,  388,
 /*  1740 */    45,  388,  388,  388,  388,  388,  388,  388,  388,  277,
 /*  1750 */   279,   81,  388,  388,  388,  388,  350,   29,   30,   31,
 /*  1760 */    32,   33,   34,   35,   36,   37,   38,   39,   40,   41,
 /*  1770 */    42,   43,   44,  388,  388,   45,  388,  409,  410,  412,
 /*  1780 */   411,  413,  388,  388,  277,  279,  217,  388,  388,  388,
 /*  1790 */   388,  350,   29,   30,   31,   32,   33,   34,   35,   36,
 /*  1800 */    37,   38,   39,   40,   41,   42,   43,   44,  507,  388,
 /*  1810 */   415,  450,  388,  444,  388,   45,  388,  414,  416,  277,
 /*  1820 */   279,  388,  388,  388,  388,  388,  354,  388,  388,  388,
 /*  1830 */   388,  350,   29,   30,   31,   32,   33,   34,   35,   36,
 /*  1840 */    37,   38,   39,   40,   41,   42,   43,   44,  388,  388,
 /*  1850 */    45,  388,  388,  388,  388,  388,  388,  388,  388,  277,
 /*  1860 */   279,  361,  388,  388,  388,  388,  350,   29,   30,   31,
 /*  1870 */    32,   33,   34,   35,   36,   37,   38,   39,   40,   41,
 /*  1880 */    42,   43,   44,  388,  388,   45,  388,  409,  410,  412,
 /*  1890 */   411,  413,  388,  388,  277,  279,  366,  388,  388,  388,
 /*  1900 */   388,  350,   29,   30,   31,   32,   33,   34,   35,   36,
 /*  1910 */    37,   38,   39,   40,   41,   42,   43,   44,  511,  388,
 /*  1920 */   415,  438,  388,  444,  388,   45,  388,  414,  416,  277,
 /*  1930 */   279,  388,  388,  388,  388,  388,  376,  388,  388,  388,
 /*  1940 */   388,  350,   29,   30,   31,   32,   33,   34,   35,   36,
 /*  1950 */    37,   38,   39,   40,   41,   42,   43,   44,  388,  388,
 /*  1960 */    45,  388,  388,  388,  388,  388,  388,  388,  388,  277,
 /*  1970 */   279,  382,  388,  388,  388,  388,  350,   29,   30,   31,
 /*  1980 */    32,   33,   34,   35,   36,   37,   38,   39,   40,   41,
 /*  1990 */    42,   43,   44,  388,  388,   45,  388,  409,  410,  412,
 /*  2000 */   411,  413,  388,  388,  277,  279,  389,  388,  388,  388,
 /*  2010 */   388,  350,   29,   30,   31,   32,   33,   34,   35,   36,
 /*  2020 */    37,   38,   39,   40,   41,   42,   43,   44,  508,  388,
 /*  2030 */   415,  450,  388,  444,  388,   45,  388,  414,  416,  277,
 /*  2040 */   279,  388,  388,  388,  388,  388,  425,  388,  388,  388,
 /*  2050 */   388,  350,   29,   30,   31,   32,   33,   34,   35,   36,
 /*  2060 */    37,   38,   39,   40,   41,   42,   43,   44,  388,  388,
 /*  2070 */    45,  388,  409,  410,  412,  411,  413,  388,  388,  277,
 /*  2080 */   279,  431,  388,  388,  388,  388,  350,   29,   30,   31,
 /*  2090 */    32,   33,   34,   35,   36,   37,   38,   39,   40,   41,
 /*  2100 */    42,   43,   44,  476,  388,  415,  450,  388,  444,  388,
 /*  2110 */   388,  388,  414,  416,  277,  279,   89,  143,  147,  141,
 /*  2120 */   179,   86,  192,  388,  136,  388,  388,  388,  388,  388,
 /*  2130 */   388,  388,  388,  388,  149,  388,  124,   89,  143,  147,
 /*  2140 */   141,  179,   86,  182,  388,  136,  388,  388,  388,  388,
 /*  2150 */   388,  388,  388,  388,  388,  149,  388,  124,  388,   89,
 /*  2160 */   143,  147,  141,  179,   86,  198,  157,  136,  388,  388,
 /*  2170 */   388,  388,  388,  388,  150,  151,  388,  149,  388,  124,
 /*  2180 */   388,  388,  152,  153,  154,  155,  156,  157,  388,  388,
 /*  2190 */   388,  388,  388,  388,  388,  150,  151,  388,  388,  388,
 /*  2200 */   388,  388,  388,  152,  153,  154,  155,  156,  388,  157,
 /*  2210 */   388,  388,  388,  388,  388,  388,  388,  150,  151,  388,
 /*  2220 */   388,  388,  388,  388,  388,  152,  153,  154,  155,  156,
 /*  2230 */    89,  143,  147,  141,  179,   86,  210,  388,  136,  388,
 /*  2240 */   388,  388,  388,  388,  388,  388,  388,  388,  149,  388,
 /*  2250 */   124,   89,  143,  147,  141,  179,   86,  388,  388,  136,
 /*  2260 */   388,  388,  388,  388,  388,  388,  388,  388,  388,  149,
 /*  2270 */   388,  124,  388,  388,   89,  143,  147,  141,  179,   86,
 /*  2280 */   157,  388,  136,  388,  388,  388,  388,  388,  150,  151,
 /*  2290 */   388,  388,  149,  351,  124,  388,  152,  153,  154,  155,
 /*  2300 */   156,  157,  388,  388,  388,  388,  388,  388,  388,  150,
 /*  2310 */   151,  388,  388,  388,  388,  388,  388,  152,  153,  154,
 /*  2320 */   155,  156,  388,  388,  157,  239,  388,  388,  388,  388,
 /*  2330 */   388,  388,  150,  151,  388,  388,  388,  388,  388,  388,
 /*  2340 */   152,  153,  154,  155,  156,   89,  143,  147,  141,  179,
 /*  2350 */    86,  388,  388,  136,  388,  388,  388,  388,  388,  388,
 /*  2360 */   388,  388,  388,  149,  388,  124,   89,  143,  147,  141,
 /*  2370 */   179,   86,  388,  388,  136,  388,  388,  388,  388,  388,
 /*  2380 */   388,  388,  388,  388,  149,  388,  124,  388,  388,   89,
 /*  2390 */   143,  147,  141,  179,   86,  157,  252,  136,  388,  388,
 /*  2400 */   388,  388,  388,  150,  151,  388,  388,  149,  388,  124,
 /*  2410 */   388,  152,  153,  154,  155,  156,  157,  260,  388,  388,
 /*  2420 */   388,  388,  388,  388,  150,  151,  388,  388,  388,  388,
 /*  2430 */   388,  363,  152,  153,  154,  155,  156,  388,  388,  157,
 /*  2440 */   388,  388,  388,  388,  388,  388,  388,  150,  151,  388,
 /*  2450 */   388,  388,  388,  388,  388,  152,  153,  154,  155,  156,
 /*  2460 */    89,  143,  147,  141,  179,   86,  388,  388,  136,  388,
 /*  2470 */   388,  388,  388,  388,  388,  388,  388,  388,  149,  388,
 /*  2480 */   124,  388,  388,  388,  388,  388,  120,  122,   88,  102,
 /*  2490 */   106,  108,  110,  112,  104,  100,  114,  116,  118,  388,
 /*  2500 */   388,  388,  388,  388,  388,  132,  388,  388,  388,  388,
 /*  2510 */   157,  388,  388,  388,  388,  388,  388,  388,  150,  151,
 /*  2520 */   388,  388,  388,  225,  388,  388,  152,  153,  154,  155,
 /*  2530 */   156,  388,  388,  388,  120,  122,   88,  102,  106,  108,
 /*  2540 */   110,  112,  104,  100,  114,  116,  118,  388,  388,  388,
 /*  2550 */   388,  226,  227,  228,  229,  230,  234,  259,  216,  247,
 /*  2560 */   267,  268,  388,  388,  388,  388,  120,  122,   88,  102,
 /*  2570 */   106,  108,  110,  112,  104,  100,  114,  116,  118,  120,
 /*  2580 */   122,   88,  102,  106,  108,  110,  112,  104,  100,  114,
 /*  2590 */   116,  118,   13,   16,  464,  470,  469,  457,  388,  463,
 /*  2600 */   437,  283,  388,  433,  388,  388,  388,  388,  388,  120,
 /*  2610 */   122,   88,  102,  106,  108,  110,  112,  104,  100,  114,
 /*  2620 */   116,  118,  388,  409,  410,  412,  411,  413,  388,  388,
 /*  2630 */   624,  388,  388,  353,  388,  388,  388,  388,  120,  122,
 /*  2640 */    88,  102,  106,  108,  110,  112,  104,  100,  114,  116,
 /*  2650 */   118,  388,  388,  388,  483,  388,  415,  450,  388,  444,
 /*  2660 */   388,  388,  360,  414,  416,  388,  388,  388,  388,  388,
 /*  2670 */   120,  122,   88,  102,  106,  108,  110,  112,  104,  100,
 /*  2680 */   114,  116,  118,  388,  409,  410,  412,  411,  413,  388,
 /*  2690 */   388,  388,  388,  388,  365,  388,  388,  388,  388,  388,
 /*  2700 */   388,  388,  120,  122,   88,  102,  106,  108,  110,  112,
 /*  2710 */   104,  100,  114,  116,  118,  489,  388,  415,  450,  388,
 /*  2720 */   444,  388,  388,  388,  414,  416,  409,  410,  412,  411,
 /*  2730 */   413,  409,  410,  412,  411,  413,  388,  388,  474,   16,
 /*  2740 */   464,  470,  469,  457,  388,  463,  437,  388,  388,  433,
 /*  2750 */   388,  409,  410,  412,  411,  413,  388,  514,  452,  415,
 /*  2760 */   417,  388,  388,  388,  415,  388,  414,  416,  388,  419,
 /*  2770 */   388,  414,  416,  409,  410,  412,  411,  413,  481,   16,
 /*  2780 */   464,  470,  469,  457,  415,  463,  437,  388,  388,  433,
 /*  2790 */   388,  414,  416,  388,  388,  388,  388,  388,  388,  388,
 /*  2800 */   458,  388,  388,  388,  388,  388,  415,  388,  388,  388,
 /*  2810 */   388,   20,  388,  414,  416,  409,  410,  412,  411,  413,
 /*  2820 */   409,  410,  412,  411,  413,  487,   16,  464,  470,  469,
 /*  2830 */   457,  388,  463,  437,  388,  388,  433,  388,  388,  388,
 /*  2840 */   388,  388,  388,  388,  388,  388,  512,  388,  415,  467,
 /*  2850 */   388,  515,  388,  415,  417,  414,  416,  388,  388,  388,
 /*  2860 */   414,  416,  409,  410,  412,  411,  413,  409,  410,  412,
 /*  2870 */   411,  413,  409,  410,  412,  411,  413,  388,  409,  410,
 /*  2880 */   412,  411,  413,  388,  409,  410,  412,  411,  413,  388,
 /*  2890 */   388,  388,  388,  513,  388,  415,  417,  388,  509,  388,
 /*  2900 */   415,  417,  414,  416,  388,  415,  388,  414,  416,  388,
 /*  2910 */    20,  415,  414,  416,  388,  388,  419,  415,  414,  416,
 /*  2920 */   439,  388,  388,  388,  414,  416,
};
static YYCODETYPE yy_lookahead[] = {
 /*     0 */    28,   24,   78,   79,   32,   28,   31,   35,   33,   11,
 /*    10 */    28,   87,   88,   89,   90,   43,   44,   45,   46,  134,
 /*    20 */    48,   49,   50,   51,   26,   53,   28,  142,  143,   57,
 /*    30 */    58,   59,   60,    0,   40,   63,   95,   96,   97,   98,
 /*    40 */    28,   43,   44,   45,   46,   47,   48,   75,   76,   77,
 /*    50 */    38,  105,   80,   81,   82,   83,   84,   85,   28,   28,
 /*    60 */    78,   79,   32,   69,   70,   35,   72,   11,   28,   87,
 /*    70 */    88,   89,   90,   43,   44,   45,   46,  134,   48,   49,
 /*    80 */    50,   51,   26,   53,   28,  142,  143,   57,   58,   59,
 /*    90 */    60,   29,   11,   63,  148,  105,  150,  151,  152,   43,
 /*   100 */    44,   45,   46,   47,   48,   75,   76,   77,   28,   28,
 /*   110 */    80,   81,   82,   83,   84,   85,   28,    6,   78,   79,
 /*   120 */    32,  132,  133,   35,   43,   44,   45,   46,   47,   48,
 /*   130 */     6,   43,   44,   45,   46,  134,   48,   49,   50,   51,
 /*   140 */    29,   53,  152,  142,  143,   57,   58,   59,   60,  110,
 /*   150 */   149,   63,    0,  135,  136,   92,   93,   94,   95,   96,
 /*   160 */    97,   98,   32,   75,   76,   77,   69,   70,   80,   81,
 /*   170 */    82,   83,   84,   85,   28,  102,  103,  104,   32,   27,
 /*   180 */   107,   35,   30,  110,   28,   61,   34,   35,    6,   43,
 /*   190 */    44,   45,   46,  134,   48,   49,   50,   51,  139,   53,
 /*   200 */    28,  142,  143,   57,   58,   59,   60,  131,   26,   63,
 /*   210 */   134,  101,  102,  103,  104,   28,  106,  107,  142,  143,
 /*   220 */   110,   75,   76,   77,  145,  146,   80,   81,   82,   83,
 /*   230 */    84,   85,   28,   40,   78,   79,   32,   69,   31,   35,
 /*   240 */    72,   36,   29,   87,   88,   89,   90,   43,   44,   45,
 /*   250 */    46,  134,   48,   49,   50,   51,  139,   53,    6,  142,
 /*   260 */   143,   57,   58,   59,   60,   78,   79,   63,  134,   64,
 /*   270 */    65,   66,   67,   68,   32,   70,  142,  143,  144,   75,
 /*   280 */    76,   77,   29,  149,   80,   81,   82,   83,   84,   85,
 /*   290 */    28,   17,   18,   19,   32,  134,   54,   35,   56,   36,
 /*   300 */    31,  140,  141,  142,  143,   43,   44,   45,   46,  134,
 /*   310 */    48,   49,   50,   51,  139,   53,   52,  142,  143,   57,
 /*   320 */    58,   59,   60,   71,   31,   63,  134,   64,   65,   66,
 /*   330 */    67,   68,   31,   70,  142,  143,  144,   75,   76,   77,
 /*   340 */    28,  149,   80,   81,   82,   83,   84,   85,   28,  102,
 /*   350 */   133,  104,   32,  106,  107,   35,  108,  110,    6,  111,
 /*   360 */   112,  113,  105,   43,   44,   45,   46,  134,   48,   49,
 /*   370 */    50,   51,   55,   53,  141,  142,  143,   57,   58,   59,
 /*   380 */    60,   29,  100,   63,  102,  103,  104,   29,  106,  107,
 /*   390 */    78,   79,  110,    6,   36,   75,   76,   77,   31,  134,
 /*   400 */    80,   81,   82,   83,   84,   85,   28,  142,  143,  144,
 /*   410 */    32,  134,    6,   35,  149,   36,   29,   31,  112,  142,
 /*   420 */   143,   43,   44,   45,   46,  134,   48,   49,   50,   51,
 /*   430 */    31,   53,   26,  142,  143,   57,   58,   59,   60,   58,
 /*   440 */   134,   63,  134,   64,   65,   66,   67,   68,  142,  143,
 /*   450 */   142,  143,  144,   75,   76,   77,    6,  149,   80,   81,
 /*   460 */    82,   83,   84,   85,   28,    6,   27,   29,   32,   30,
 /*   470 */     6,   35,  104,   34,   35,  107,   26,    6,  110,   43,
 /*   480 */    44,   45,   46,  134,   48,   49,   50,   51,   29,   53,
 /*   490 */    26,  142,  143,   57,   58,   59,   60,   26,  134,   63,
 /*   500 */   134,   32,  138,  105,  145,  146,  142,  143,  142,  143,
 /*   510 */   144,   75,   76,   77,    6,  149,   80,   81,   82,   83,
 /*   520 */    84,   85,   28,   54,  131,   56,   32,  134,   28,   35,
 /*   530 */   111,  112,  113,    6,   26,  142,  143,   43,   44,   45,
 /*   540 */    46,  147,   48,   49,   50,   51,   70,   53,  150,  151,
 /*   550 */   152,   57,   58,   59,   60,  131,   29,   63,  134,  145,
 /*   560 */   146,  131,  145,  146,  134,    6,  142,  143,   40,   75,
 /*   570 */    76,   77,  142,  143,   80,   81,   82,   83,   84,   85,
 /*   580 */    28,  134,   71,  134,   32,  138,    6,   35,   29,  142,
 /*   590 */   143,  142,  143,  145,  146,   43,   44,   45,   46,  134,
 /*   600 */    48,   49,   50,   51,   71,   53,   26,  142,  143,   57,
 /*   610 */    58,   59,   60,   71,  134,   63,  134,   31,  138,   33,
 /*   620 */   138,    6,  142,  143,  142,  143,   26,   75,   76,   77,
 /*   630 */    28,  134,   80,   81,   82,   83,   84,   85,   28,  142,
 /*   640 */   143,  134,   32,  134,   29,   35,   28,  138,    6,  142,
 /*   650 */   143,  142,  143,   43,   44,   45,   46,  134,   48,   49,
 /*   660 */    50,   51,  147,   53,   55,  142,  143,   57,   58,   59,
 /*   670 */    60,  134,   55,   63,  134,  138,  145,  146,  138,  142,
 /*   680 */   143,    6,  142,  143,   55,   75,   76,   77,   55,  134,
 /*   690 */    80,   81,   82,   83,   84,   85,   28,  142,  143,  134,
 /*   700 */    32,  134,   28,   35,   29,  138,   40,  142,  143,  142,
 /*   710 */   143,   43,   44,   45,   46,  134,   48,   49,   50,   51,
 /*   720 */    40,   53,   71,  142,  143,   57,   58,   59,   60,  131,
 /*   730 */   134,   63,  134,   71,  138,  145,  146,   28,  142,  143,
 /*   740 */   142,  143,   28,   75,   76,   77,   28,  134,   80,   81,
 /*   750 */    82,   83,   84,   85,   28,  142,  143,    6,   32,  134,
 /*   760 */   108,   35,   71,  111,  112,  113,  136,  142,  143,   43,
 /*   770 */    44,   45,   46,  134,   48,   49,   50,   51,  137,   53,
 /*   780 */    29,  142,  143,   57,   58,   59,   60,  134,   71,   63,
 /*   790 */   134,  145,  146,  145,  146,  142,  143,    6,  142,  143,
 /*   800 */   137,   75,   76,   77,  137,  134,   80,   81,   82,   83,
 /*   810 */    84,   85,   28,  142,  143,  134,   32,  134,   28,   35,
 /*   820 */    29,  137,    6,  142,  143,  142,  143,   43,   44,   45,
 /*   830 */    46,  134,   48,   49,   50,   51,  137,   53,  137,  142,
 /*   840 */   143,   57,   58,   59,   60,   29,   29,   63,  104,  145,
 /*   850 */   146,   71,    6,   36,  110,   29,  104,  137,  137,   75,
 /*   860 */    76,   77,  110,   71,   80,   81,   82,   83,   84,   85,
 /*   870 */    28,  137,   29,   28,   32,   29,   28,   35,   29,   71,
 /*   880 */    29,   71,   29,   71,   29,   43,   44,   45,   46,   29,
 /*   890 */    48,   49,   50,   51,   28,   53,   40,   29,   28,   57,
 /*   900 */    58,   59,   60,  146,  105,   63,   28,   36,   28,   28,
 /*   910 */    58,   61,   29,   55,   52,   31,   31,   75,   76,   77,
 /*   920 */   105,   12,   80,   81,   82,   83,   84,   85,   28,   36,
 /*   930 */    28,   36,   32,  105,   28,   35,  105,   36,   28,   28,
 /*   940 */    28,   38,  105,   43,   44,   45,   46,   40,   48,   49,
 /*   950 */    50,   51,   31,   53,   31,   36,   29,   57,   58,   59,
 /*   960 */    60,   36,   38,   63,  105,   29,  105,   29,  105,   29,
 /*   970 */    28,   32,   31,   30,   28,   75,   76,   77,   28,   32,
 /*   980 */    80,   81,   82,   83,   84,   85,   28,   31,  153,   32,
 /*   990 */    32,  153,  153,   35,  153,  153,  153,  153,  153,  153,
 /*  1000 */   153,   43,   44,   45,   46,  153,   48,   49,   50,   51,
 /*  1010 */   153,   53,  153,  153,  153,   57,   58,   59,   60,  153,
 /*  1020 */   153,   63,  153,  153,  153,  153,  153,  153,  153,  153,
 /*  1030 */   153,  153,  153,   75,   76,   77,  153,  153,   80,   81,
 /*  1040 */    82,   83,   84,   85,   28,  153,  153,  153,   32,  153,
 /*  1050 */   153,   35,  153,  153,  153,  153,  153,  153,  153,   43,
 /*  1060 */    44,   45,   46,  153,   48,   49,   50,   51,  153,   53,
 /*  1070 */   153,  153,  153,   57,   58,   59,   60,  153,  153,   63,
 /*  1080 */   153,  153,  153,  153,  153,  153,  153,  153,  153,  153,
 /*  1090 */   153,   75,   76,   77,  153,  153,   80,   81,   82,   83,
 /*  1100 */    84,   85,   28,  153,  153,  153,   32,  153,  153,   35,
 /*  1110 */   153,  153,  153,  153,  153,  153,  153,   43,   44,   45,
 /*  1120 */    46,  153,   48,   49,   50,   51,  153,   53,  153,  153,
 /*  1130 */   153,   57,   58,   59,   60,  153,  153,   63,  153,  153,
 /*  1140 */   153,  153,  153,  153,  153,  153,  153,  153,  153,   75,
 /*  1150 */    76,   77,  153,  153,   80,   81,   82,   83,   84,   85,
 /*  1160 */    28,  153,  153,  153,   32,  153,  153,   35,  153,  153,
 /*  1170 */   153,  153,  153,  153,  153,   43,   44,   45,   46,  153,
 /*  1180 */    48,   49,   50,   51,  153,   53,  153,  153,  153,   57,
 /*  1190 */    58,   59,   60,  153,  153,   63,  153,  153,  153,  153,
 /*  1200 */   153,  153,  153,  153,  153,  153,  153,   75,   76,   77,
 /*  1210 */   153,  153,   80,   81,   82,   83,   84,   85,   28,  153,
 /*  1220 */   153,  153,   32,  153,  153,   35,  153,  153,  153,  153,
 /*  1230 */   153,  153,  153,   43,   44,   45,   46,  153,   48,   49,
 /*  1240 */    50,   51,  153,   53,  153,  153,  153,   57,   58,   59,
 /*  1250 */    60,  153,  153,   63,  153,  153,  153,  153,  153,  153,
 /*  1260 */   153,  153,  153,  153,  153,   75,   76,   77,  153,  153,
 /*  1270 */    80,   81,   82,   83,   84,   85,   28,  153,  153,  153,
 /*  1280 */    32,  153,  153,   35,  153,  153,  153,  153,  153,  153,
 /*  1290 */   153,   43,   44,   45,   46,  153,   48,   49,   50,   51,
 /*  1300 */   153,   53,  153,  153,  153,   57,   58,   59,   60,  153,
 /*  1310 */   153,   63,  153,  153,  153,  153,  153,  153,  153,  153,
 /*  1320 */   153,  153,  153,   75,   76,   77,  153,  153,   80,   81,
 /*  1330 */    82,   83,   84,   85,   28,  153,  153,  153,   32,  153,
 /*  1340 */   153,   35,  153,  153,  153,  153,  153,  153,  153,   43,
 /*  1350 */    44,   45,   46,  153,   48,   49,   50,   51,  153,   53,
 /*  1360 */   153,  153,  153,   57,   58,   59,   60,  153,  153,   63,
 /*  1370 */   153,    1,    2,    3,    4,    5,  153,  153,   28,  153,
 /*  1380 */   153,   75,   76,   77,  153,   35,   80,   81,   82,   83,
 /*  1390 */    84,   85,  153,   43,   44,   45,   46,  153,   48,   49,
 /*  1400 */    50,   51,   32,   53,   34,   35,  153,   57,   58,   59,
 /*  1410 */    60,   41,   42,   63,  153,  153,  153,  153,  153,  153,
 /*  1420 */   153,  153,  153,  153,  153,   75,   76,   77,  153,  153,
 /*  1430 */    80,   81,   82,   83,   84,   85,  153,  153,  153,  153,
 /*  1440 */   153,  153,  153,  153,  153,  153,   20,   21,   22,   23,
 /*  1450 */    24,   25,  153,  153,   28,  153,  153,  153,  153,   28,
 /*  1460 */   153,  153,  153,   98,   38,  153,   40,  153,  153,   43,
 /*  1470 */    44,   45,   46,   47,   48,  153,  153,  153,  153,  114,
 /*  1480 */   115,  116,  117,  118,  119,  120,  121,  122,  123,  124,
 /*  1490 */   125,  126,  127,  128,  129,  130,   70,  153,  153,  153,
 /*  1500 */   153,  153,   71,  153,   78,   79,   98,  142,  143,   78,
 /*  1510 */    79,  153,   86,   87,   88,   89,   90,  109,   87,   88,
 /*  1520 */    89,   90,  114,  115,  116,  117,  118,  119,  120,  121,
 /*  1530 */   122,  123,  124,  125,  126,  127,  128,  129,  130,  153,
 /*  1540 */   153,  153,  153,  153,  153,   98,  153,  153,  153,  153,
 /*  1550 */   142,  143,  153,  153,  153,  153,  109,  153,  153,  153,
 /*  1560 */   153,  114,  115,  116,  117,  118,  119,  120,  121,  122,
 /*  1570 */   123,  124,  125,  126,  127,  128,  129,  130,    9,   10,
 /*  1580 */    11,   12,   13,   14,   15,   16,   17,   18,   19,  142,
 /*  1590 */   143,  153,  153,  153,  153,   98,  153,  153,  153,  153,
 /*  1600 */   153,  153,  153,  153,  153,  153,  109,  153,  153,  153,
 /*  1610 */   153,  114,  115,  116,  117,  118,  119,  120,  121,  122,
 /*  1620 */   123,  124,  125,  126,  127,  128,  129,  130,  153,  153,
 /*  1630 */    98,  153,  153,  153,  153,  153,  153,  153,  153,  142,
 /*  1640 */   143,  109,  153,  153,  153,  153,  114,  115,  116,  117,
 /*  1650 */   118,  119,  120,  121,  122,  123,  124,  125,  126,  127,
 /*  1660 */   128,  129,  130,  153,  153,   98,  153,    1,    2,    3,
 /*  1670 */     4,    5,  153,  153,  142,  143,  109,  153,  153,  153,
 /*  1680 */   153,  114,  115,  116,  117,  118,  119,  120,  121,  122,
 /*  1690 */   123,  124,  125,  126,  127,  128,  129,  130,   32,  153,
 /*  1700 */    34,   35,  153,   37,  153,   98,  153,   41,   42,  142,
 /*  1710 */   143,  153,  153,  153,  153,  153,  109,  153,  153,  153,
 /*  1720 */   153,  114,  115,  116,  117,  118,  119,  120,  121,  122,
 /*  1730 */   123,  124,  125,  126,  127,  128,  129,  130,  153,  153,
 /*  1740 */    98,  153,  153,  153,  153,  153,  153,  153,  153,  142,
 /*  1750 */   143,  109,  153,  153,  153,  153,  114,  115,  116,  117,
 /*  1760 */   118,  119,  120,  121,  122,  123,  124,  125,  126,  127,
 /*  1770 */   128,  129,  130,  153,  153,   98,  153,    1,    2,    3,
 /*  1780 */     4,    5,  153,  153,  142,  143,  109,  153,  153,  153,
 /*  1790 */   153,  114,  115,  116,  117,  118,  119,  120,  121,  122,
 /*  1800 */   123,  124,  125,  126,  127,  128,  129,  130,   32,  153,
 /*  1810 */    34,   35,  153,   37,  153,   98,  153,   41,   42,  142,
 /*  1820 */   143,  153,  153,  153,  153,  153,  109,  153,  153,  153,
 /*  1830 */   153,  114,  115,  116,  117,  118,  119,  120,  121,  122,
 /*  1840 */   123,  124,  125,  126,  127,  128,  129,  130,  153,  153,
 /*  1850 */    98,  153,  153,  153,  153,  153,  153,  153,  153,  142,
 /*  1860 */   143,  109,  153,  153,  153,  153,  114,  115,  116,  117,
 /*  1870 */   118,  119,  120,  121,  122,  123,  124,  125,  126,  127,
 /*  1880 */   128,  129,  130,  153,  153,   98,  153,    1,    2,    3,
 /*  1890 */     4,    5,  153,  153,  142,  143,  109,  153,  153,  153,
 /*  1900 */   153,  114,  115,  116,  117,  118,  119,  120,  121,  122,
 /*  1910 */   123,  124,  125,  126,  127,  128,  129,  130,   32,  153,
 /*  1920 */    34,   35,  153,   37,  153,   98,  153,   41,   42,  142,
 /*  1930 */   143,  153,  153,  153,  153,  153,  109,  153,  153,  153,
 /*  1940 */   153,  114,  115,  116,  117,  118,  119,  120,  121,  122,
 /*  1950 */   123,  124,  125,  126,  127,  128,  129,  130,  153,  153,
 /*  1960 */    98,  153,  153,  153,  153,  153,  153,  153,  153,  142,
 /*  1970 */   143,  109,  153,  153,  153,  153,  114,  115,  116,  117,
 /*  1980 */   118,  119,  120,  121,  122,  123,  124,  125,  126,  127,
 /*  1990 */   128,  129,  130,  153,  153,   98,  153,    1,    2,    3,
 /*  2000 */     4,    5,  153,  153,  142,  143,  109,  153,  153,  153,
 /*  2010 */   153,  114,  115,  116,  117,  118,  119,  120,  121,  122,
 /*  2020 */   123,  124,  125,  126,  127,  128,  129,  130,   32,  153,
 /*  2030 */    34,   35,  153,   37,  153,   98,  153,   41,   42,  142,
 /*  2040 */   143,  153,  153,  153,  153,  153,  109,  153,  153,  153,
 /*  2050 */   153,  114,  115,  116,  117,  118,  119,  120,  121,  122,
 /*  2060 */   123,  124,  125,  126,  127,  128,  129,  130,  153,  153,
 /*  2070 */    98,  153,    1,    2,    3,    4,    5,  153,  153,  142,
 /*  2080 */   143,  109,  153,  153,  153,  153,  114,  115,  116,  117,
 /*  2090 */   118,  119,  120,  121,  122,  123,  124,  125,  126,  127,
 /*  2100 */   128,  129,  130,   32,  153,   34,   35,  153,   37,  153,
 /*  2110 */   153,  153,   41,   42,  142,  143,   20,   21,   22,   23,
 /*  2120 */    24,   25,   26,  153,   28,  153,  153,  153,  153,  153,
 /*  2130 */   153,  153,  153,  153,   38,  153,   40,   20,   21,   22,
 /*  2140 */    23,   24,   25,   26,  153,   28,  153,  153,  153,  153,
 /*  2150 */   153,  153,  153,  153,  153,   38,  153,   40,  153,   20,
 /*  2160 */    21,   22,   23,   24,   25,   26,   70,   28,  153,  153,
 /*  2170 */   153,  153,  153,  153,   78,   79,  153,   38,  153,   40,
 /*  2180 */   153,  153,   86,   87,   88,   89,   90,   70,  153,  153,
 /*  2190 */   153,  153,  153,  153,  153,   78,   79,  153,  153,  153,
 /*  2200 */   153,  153,  153,   86,   87,   88,   89,   90,  153,   70,
 /*  2210 */   153,  153,  153,  153,  153,  153,  153,   78,   79,  153,
 /*  2220 */   153,  153,  153,  153,  153,   86,   87,   88,   89,   90,
 /*  2230 */    20,   21,   22,   23,   24,   25,   26,  153,   28,  153,
 /*  2240 */   153,  153,  153,  153,  153,  153,  153,  153,   38,  153,
 /*  2250 */    40,   20,   21,   22,   23,   24,   25,  153,  153,   28,
 /*  2260 */   153,  153,  153,  153,  153,  153,  153,  153,  153,   38,
 /*  2270 */   153,   40,  153,  153,   20,   21,   22,   23,   24,   25,
 /*  2280 */    70,  153,   28,  153,  153,  153,  153,  153,   78,   79,
 /*  2290 */   153,  153,   38,   62,   40,  153,   86,   87,   88,   89,
 /*  2300 */    90,   70,  153,  153,  153,  153,  153,  153,  153,   78,
 /*  2310 */    79,  153,  153,  153,  153,  153,  153,   86,   87,   88,
 /*  2320 */    89,   90,  153,  153,   70,   71,  153,  153,  153,  153,
 /*  2330 */   153,  153,   78,   79,  153,  153,  153,  153,  153,  153,
 /*  2340 */    86,   87,   88,   89,   90,   20,   21,   22,   23,   24,
 /*  2350 */    25,  153,  153,   28,  153,  153,  153,  153,  153,  153,
 /*  2360 */   153,  153,  153,   38,  153,   40,   20,   21,   22,   23,
 /*  2370 */    24,   25,  153,  153,   28,  153,  153,  153,  153,  153,
 /*  2380 */   153,  153,  153,  153,   38,  153,   40,  153,  153,   20,
 /*  2390 */    21,   22,   23,   24,   25,   70,   71,   28,  153,  153,
 /*  2400 */   153,  153,  153,   78,   79,  153,  153,   38,  153,   40,
 /*  2410 */   153,   86,   87,   88,   89,   90,   70,   71,  153,  153,
 /*  2420 */   153,  153,  153,  153,   78,   79,  153,  153,  153,  153,
 /*  2430 */   153,   62,   86,   87,   88,   89,   90,  153,  153,   70,
 /*  2440 */   153,  153,  153,  153,  153,  153,  153,   78,   79,  153,
 /*  2450 */   153,  153,  153,  153,  153,   86,   87,   88,   89,   90,
 /*  2460 */    20,   21,   22,   23,   24,   25,  153,  153,   28,  153,
 /*  2470 */   153,  153,  153,  153,  153,  153,  153,  153,   38,  153,
 /*  2480 */    40,  153,  153,  153,  153,  153,    7,    8,    9,   10,
 /*  2490 */    11,   12,   13,   14,   15,   16,   17,   18,   19,  153,
 /*  2500 */   153,  153,  153,  153,  153,   26,  153,  153,  153,  153,
 /*  2510 */    70,  153,  153,  153,  153,  153,  153,  153,   78,   79,
 /*  2520 */   153,  153,  153,   36,  153,  153,   86,   87,   88,   89,
 /*  2530 */    90,  153,  153,  153,    7,    8,    9,   10,   11,   12,
 /*  2540 */    13,   14,   15,   16,   17,   18,   19,  153,  153,  153,
 /*  2550 */   153,   64,   65,   66,   67,   68,   69,   70,   31,   72,
 /*  2560 */    73,   74,  153,  153,  153,  153,    7,    8,    9,   10,
 /*  2570 */    11,   12,   13,   14,   15,   16,   17,   18,   19,    7,
 /*  2580 */     8,    9,   10,   11,   12,   13,   14,   15,   16,   17,
 /*  2590 */    18,   19,   99,  100,  101,  102,  103,  104,  153,  106,
 /*  2600 */   107,   29,  153,  110,  153,  153,  153,  153,  153,    7,
 /*  2610 */     8,    9,   10,   11,   12,   13,   14,   15,   16,   17,
 /*  2620 */    18,   19,  153,    1,    2,    3,    4,    5,  153,  153,
 /*  2630 */    71,  153,  153,   31,  153,  153,  153,  153,    7,    8,
 /*  2640 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*  2650 */    19,  153,  153,  153,   32,  153,   34,   35,  153,   37,
 /*  2660 */   153,  153,   31,   41,   42,  153,  153,  153,  153,  153,
 /*  2670 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*  2680 */    17,   18,   19,  153,    1,    2,    3,    4,    5,  153,
 /*  2690 */   153,  153,  153,  153,   31,  153,  153,  153,  153,  153,
 /*  2700 */   153,  153,    7,    8,    9,   10,   11,   12,   13,   14,
 /*  2710 */    15,   16,   17,   18,   19,   32,  153,   34,   35,  153,
 /*  2720 */    37,  153,  153,  153,   41,   42,    1,    2,    3,    4,
 /*  2730 */     5,    1,    2,    3,    4,    5,  153,  153,   99,  100,
 /*  2740 */   101,  102,  103,  104,  153,  106,  107,  153,  153,  110,
 /*  2750 */   153,    1,    2,    3,    4,    5,  153,   32,   28,   34,
 /*  2760 */    35,  153,  153,  153,   34,  153,   41,   42,  153,   39,
 /*  2770 */   153,   41,   42,    1,    2,    3,    4,    5,   99,  100,
 /*  2780 */   101,  102,  103,  104,   34,  106,  107,  153,  153,  110,
 /*  2790 */   153,   41,   42,  153,  153,  153,  153,  153,  153,  153,
 /*  2800 */    28,  153,  153,  153,  153,  153,   34,  153,  153,  153,
 /*  2810 */   153,   39,  153,   41,   42,    1,    2,    3,    4,    5,
 /*  2820 */     1,    2,    3,    4,    5,   99,  100,  101,  102,  103,
 /*  2830 */   104,  153,  106,  107,  153,  153,  110,  153,  153,  153,
 /*  2840 */   153,  153,  153,  153,  153,  153,   32,  153,   34,   35,
 /*  2850 */   153,   32,  153,   34,   35,   41,   42,  153,  153,  153,
 /*  2860 */    41,   42,    1,    2,    3,    4,    5,    1,    2,    3,
 /*  2870 */     4,    5,    1,    2,    3,    4,    5,  153,    1,    2,
 /*  2880 */     3,    4,    5,  153,    1,    2,    3,    4,    5,  153,
 /*  2890 */   153,  153,  153,   32,  153,   34,   35,  153,   32,  153,
 /*  2900 */    34,   35,   41,   42,  153,   34,  153,   41,   42,  153,
 /*  2910 */    39,   34,   41,   42,  153,  153,   39,   34,   41,   42,
 /*  2920 */    37,  153,  153,  153,   41,   42,
};
#define YY_SHIFT_USE_DFLT (-77)
static short yy_shift_ofst[] = {
 /*     0 */   439,   33,  152,  -77,  -77,  -77,  -77,   31,   62,  -77,
 /*    10 */    80,  -25, 1666,  130,  -77,  -77, 1776, 1370,  -77, 2871,
 /*    20 */   172,  193,   -2,  207,  -28,  -77,   30,  -77,  -77,  -77,
 /*    30 */   -77,  -77,  -77,  -77,  -77,  -77,  -77,  -77,  -77,  -77,
 /*    40 */   -77,  -77,  -77,  -77,  -77,  -77,  213,  -77,  253,  -77,
 /*    50 */  2440,  269,   88,  146,  264,  293,  204,  262,  -77, 2440,
 /*    60 */   301,  242,  -77,  469,  -77,  -77,  -76,  317, 1350, 1350,
 /*    70 */   367,  320,  378,  -77, 2440,  386,  436,  494,  -77,  399,
 /*    80 */   552,  610,  381, 2440,  438,  -77, 2440,  -77, 2440,  500,
 /*    90 */   -77,  476,   40,  511,  -77,  533,  -77,  542,  -77,  274,
 /*   100 */  2440,  274, 2440,  274, 2440,  274, 2440,  274, 2440,  274,
 /*   110 */  2440,  274, 2440,  274, 2440,  -77, 2440,  -77, 2440,  -77,
 /*   120 */  2440, 1569, 2440, 1569, 1426,  -77,  -77,  -77,  -77,  -77,
 /*   130 */   -77, 2479,  -77,  600, 2440,  -77,   -6,  602,  528, 2096,
 /*   140 */  2695, 2440,  -77,  618,  642,  500,  -77, 2440,  -77,  -77,
 /*   150 */   -77,  -77,  -77,  -77,  -77,  -77,  -77, 1431,  -77,  252,
 /*   160 */   -77,  -18,  -77,  609,  156,  -77,  -77,  -77,  -77,  -77,
 /*   170 */   -77,  -77,  -77,  -77,  -77,  617,  629,  633,  -77,  674,
 /*   180 */   666, 2117,  -77,  182,  -77, 2440,  -77,  -77,  -77,  -77,
 /*   190 */   406,  -77,  -77,   12,  680, 2139,  450,  -77,  -77,  -77,
 /*   200 */   187,  651,  -77,  662,  -77,  691,  -77, 2210,  464,  -77,
 /*   210 */   -77, 2695,  709,  124, 2231, 2527, 1350,  668,  -77,  714,
 /*   220 */   111,  -77,  714,  -77, 2487,  -77,  -77,  -77,  -77,  -77,
 /*   230 */   -77, 2440,  -77, 2695,  718,  205, 2440,  -77, 2254,  379,
 /*   240 */  2440,  -77,  717,  379, 2440,  -77, 2559,  790,  263, 2440,
 /*   250 */   -77, 2325,  379, 2440,  -77,  780,  379, 2440,  -77, 2346,
 /*   260 */   379, 2440,  -77,  792,  379, 2440,  -77,  -77,  -77,  -77,
 /*   270 */  2440,  352,  -77, 2440,  -77, 2695,  -77,  826,  -77,  843,
 /*   280 */   -77, 2440, 2572,  -77,  845,   97,  848,  849,  -77,  312,
 /*   290 */   808,  851,  -77,  810,  853,  -77,  812,  855,  -77,  -23,
 /*   300 */   860,  -77,  866,  856, 2440,  471,  868,  -77,  870,  387,
 /*   310 */   -77,  870,  -77,  871,  -76,  -77,  -77,  870,  459,  -77,
 /*   320 */   870,  527,  -77,  870,  559,  -77,  870,  615,  -77,  870,
 /*   330 */   675,  -77,  870,  751,  -77,  870,  791,  -77,  870,  816,
 /*   340 */   -77,  870,  846,  -77,  168,  878,  680,  880,  528,  -77,
 /*   350 */   -77, 2440, 2602, 1350,  726,  -77,  881,  850, 2369, 2631,
 /*   360 */  1350,  784,  -77, 2440, 2663, 1350,  842,  -77,  852, 2440,
 /*   370 */   883,  -77,  -77,  -77,  858, 1350, 1350,  -77,  -77,  862,
 /*   380 */   884,  900,  958,  -77,  -77,  508,  885, 1016,  -77, 1074,
 /*   390 */   -77,   81,  -77,  893,  -76,  -77,  902,  895,  -76,  -77,
 /*   400 */   906,  901,  -76,  -77,  910,  909,  911,  -77,  -77,  -77,
 /*   410 */   -77,  -77,  -77,  -77,  -77,  -77,  -77, 2750, 2877,  912,
 /*   420 */   907,   56,  921, 1132,  -77, 1190,  -77,  580,  923, 1248,
 /*   430 */   -77, 1306,  -77,  -77, 1886, 2725,  -77,  -77, 2883,  903,
 /*   440 */   919,  -76,  927,  -77,  924,  925,  -76,  936,  -77,  -77,
 /*   450 */  2883, 2730,  358,  -77,  -76,  938,  -77, 2772,  817,  -77,
 /*   460 */   -76,  940,  -77,  -77, 1996, 2814, 2819, 2750, 2861,  -77,
 /*   470 */  2866,  942,  941, 2071,  939,  -77,  -77,  943,  946,  586,
 /*   480 */  2622,  947,  -77,  -77,  950,  956, 2683,  957,  -77,  -77,
 /*   490 */   -77,
};
#define YY_REDUCE_USE_DFLT (-116)
static short yy_reduce_ofst[] = {
 /*     0 */    63, -116,  -59, -116, -116, -116, -116, -116, -116, -116,
 /*    10 */  -116, -116, 2493, -116, -116, -116,  110,  368, -116,   39,
 /*    20 */  -116, -116,  248, -116, 1408, -116, 1365, -116, -116, -116,
 /*    30 */  -116, -116, -116, -116, -116, -116, -116, -116, -116, -116,
 /*    40 */  -116, -116, -116, -116, -116, -116, -116, -116, -116, -116,
 /*    50 */    76, -116, 1447, 1365, -116, -116, 1497, 1365, -116,  393,
 /*    60 */  -116,  -11, -116,  217, -116, -116,  257, -116, 1532, 1365,
 /*    70 */  -116, 1567, 1365, -116,  424, -116, 1607, 1365, -116, -116,
 /*    80 */  1642, 1365, -116,  430, -116, -116, -115, -116,  -57,  394,
 /*    90 */  -116, -116, -116, -116, -116, -116, -116, -116, -116, -116,
 /*   100 */   277, -116,  291, -116,  349, -116,  449, -116,  465, -116,
 /*   110 */   497, -116,  507, -116,  523, -116,  555, -116,  565, -116,
 /*   120 */   581, -116,  613, -116,  306, -116, -116, -116, -116, -116,
 /*   130 */  -116, -116, -116, -116,  625, -116, -116, -116, -116,  134,
 /*   140 */  -116,  639, -116, -116, -116,  515, -116,  653, -116, -116,
 /*   150 */  -116, -116, -116, -116, -116, -116, -116,  -54, -116, -116,
 /*   160 */  -116,  398, -116, -116,  -10, -116, -116, -116, -116, -116,
 /*   170 */  -116, -116, -116, -116, -116, -116, -116, -116, -116, -116,
 /*   180 */  -116,  192, -116, -116, -116,    1, -116, -116, -116, -116,
 /*   190 */  -116, -116, -116, -116, -116,  265, -116, -116, -116, -116,
 /*   200 */  -116, -116, -116, -116, -116, -116, -116,  308, -116, -116,
 /*   210 */  -116, -116, -116, -116,  656, -116, 1677, 1365, -116,   18,
 /*   220 */  -116, -116,  630, -116,  641, -116, -116, -116, -116, -116,
 /*   230 */  -116,  364, -116, -116, -116,  663,  447, -116,   59,  667,
 /*   240 */   480, -116, -116,  684,  482, -116, -116, -116,  699,  509,
 /*   250 */  -116,  117,  701,  537, -116, -116,  720,  540, -116,  175,
 /*   260 */   721,  567, -116, -116,  734,  596, -116, -116, -116, -116,
 /*   270 */   161, -116, -116,  233, -116, -116, -116, -116, -116, -116,
 /*   280 */  -116,  671, -116, -116, -116, -116, -116, -116, -116, -116,
 /*   290 */  -116, -116, -116, -116, -116, -116, -116, -116, -116, -116,
 /*   300 */  -116, -116, -116, -116,  366, -116, -116, -116,   79, -116,
 /*   310 */  -116,  757, -116, -116,  799, -116, -116,  359, -116, -116,
 /*   320 */   414, -116, -116,  417, -116, -116,  448, -116, -116,  531,
 /*   330 */  -116, -116,  590, -116, -116,  646, -116, -116,  648, -116,
 /*   340 */  -116,  704, -116, -116, -116, -116, -116, -116, -116, -116,
 /*   350 */  -116,  681, -116, 1717, 1365, -116, -116, -116,  683, -116,
 /*   360 */  1752, 1365, -116,  697, -116, 1787, 1365, -116, -116,  598,
 /*   370 */  -116, -116, -116, -116, -116, 1827, 1365, -116, -116, -116,
 /*   380 */  -116, 1862, 1365, -116, -116, -116, -116, 1897, -116, 1365,
 /*   390 */  -116,  419, -116, -116,  815, -116, -116, -116,  828, -116,
 /*   400 */  -116, -116,  831, -116, -116, -116, -116, -116, -116, -116,
 /*   410 */  -116, -116, -116, -116, -116, -116, -116,  744,   39, -116,
 /*   420 */  -116,  652, -116, 1937, -116, 1365, -116, -116, -116, 1972,
 /*   430 */  -116, 1365, -116, -116,  247,  368, -116, -116,  744, -116,
 /*   440 */  -116,  837, -116, -116, -116, -116,  859, -116, -116, -116,
 /*   450 */   752,   39, -116, -116,  861, -116, -116,   39, -116, -116,
 /*   460 */   863, -116, -116, -116,  282,   73,  368,  752,  368, -116,
 /*   470 */   368, -116, -116, 2639, -116, -116, -116, -116, -116, -116,
 /*   480 */  2679, -116, -116, -116, -116, -116, 2726, -116, -116, -116,
 /*   490 */  -116,
};
static YYACTIONTYPE yy_default[] = {
 /*     0 */   724,  724,  724,  493,  495,  496,  497,  724,  724,  498,
 /*    10 */   724,  724,  724,  724,  499,  503,  724,  724,  524,  724,
 /*    20 */   724,  724,  724,  724,  724,  528,  724,  530,  561,  563,
 /*    30 */   564,  565,  566,  567,  568,  569,  570,  571,  572,  573,
 /*    40 */   574,  575,  576,  577,  578,  579,  724,  580,  724,  581,
 /*    50 */   724,  724,  724,  724,  584,  724,  724,  724,  585,  724,
 /*    60 */   724,  724,  588,  724,  589,  590,  724,  724,  724,  592,
 /*    70 */   724,  724,  724,  595,  724,  724,  724,  724,  597,  724,
 /*    80 */   724,  724,  724,  724,  724,  599,  724,  653,  724,  724,
 /*    90 */   654,  724,  724,  724,  719,  724,  720,  724,  721,  655,
 /*   100 */   724,  656,  724,  657,  724,  658,  724,  659,  724,  660,
 /*   110 */   724,  661,  724,  662,  724,  671,  724,  672,  724,  673,
 /*   120 */   724,  674,  724,  675,  724,  555,  556,  557,  558,  559,
 /*   130 */   560,  724,  663,  724,  724,  664,  680,  724,  665,  724,
 /*   140 */   703,  724,  676,  724,  724,  724,  677,  724,  678,  679,
 /*   150 */   681,  682,  683,  684,  685,  686,  687,  724,  688,  724,
 /*   160 */   689,  724,  704,  724,  724,  706,  711,  712,  713,  714,
 /*   170 */   715,  716,  717,  718,  707,  712,  714,  713,  705,  724,
 /*   180 */   690,  724,  691,  724,  692,  724,  695,  696,  701,  702,
 /*   190 */   724,  699,  700,  724,  666,  724,  724,  697,  698,  667,
 /*   200 */   724,  724,  668,  724,  669,  724,  670,  724,  724,  693,
 /*   210 */   694,  722,  724,  724,  724,  724,  724,  724,  600,  724,
 /*   220 */   724,  604,  724,  605,  724,  607,  608,  609,  610,  611,
 /*   230 */   612,  724,  613,  652,  724,  724,  724,  614,  724,  724,
 /*   240 */   724,  615,  724,  724,  724,  616,  724,  724,  724,  724,
 /*   250 */   617,  724,  724,  724,  618,  724,  724,  724,  619,  724,
 /*   260 */   724,  724,  620,  724,  724,  724,  621,  622,  623,  606,
 /*   270 */   724,  724,  625,  724,  626,  628,  627,  724,  629,  724,
 /*   280 */   630,  724,  724,  631,  724,  724,  724,  724,  632,  724,
 /*   290 */   724,  724,  633,  724,  724,  634,  724,  724,  635,  724,
 /*   300 */   724,  636,  724,  724,  724,  724,  724,  637,  724,  724,
 /*   310 */   638,  724,  648,  650,  724,  651,  649,  724,  724,  639,
 /*   320 */   724,  724,  640,  724,  724,  641,  724,  724,  642,  724,
 /*   330 */   724,  643,  724,  724,  644,  724,  724,  645,  724,  724,
 /*   340 */   646,  724,  724,  647,  724,  724,  724,  724,  724,  723,
 /*   350 */   562,  724,  724,  724,  724,  601,  724,  724,  724,  724,
 /*   360 */   724,  724,  602,  724,  724,  724,  724,  603,  724,  724,
 /*   370 */   724,  598,  596,  594,  724,  724,  593,  591,  586,  582,
 /*   380 */   724,  724,  724,  587,  583,  724,  724,  724,  529,  724,
 /*   390 */   531,  724,  546,  548,  724,  551,  724,  549,  724,  552,
 /*   400 */   724,  550,  724,  553,  724,  724,  724,  547,  536,  538,
 /*   410 */   539,  540,  541,  542,  543,  544,  545,  724,  724,  724,
 /*   420 */   724,  724,  724,  724,  532,  724,  534,  724,  724,  724,
 /*   430 */   533,  724,  535,  537,  724,  724,  522,  525,  724,  724,
 /*   440 */   724,  724,  724,  526,  724,  724,  724,  724,  527,  516,
 /*   450 */   724,  724,  724,  518,  724,  724,  520,  724,  724,  519,
 /*   460 */   724,  724,  521,  523,  724,  724,  724,  724,  724,  517,
 /*   470 */   724,  724,  724,  724,  724,  500,  504,  724,  724,  724,
 /*   480 */   724,  724,  501,  505,  724,  724,  724,  724,  502,  506,
 /*   490 */   494,
};
#define YY_SZ_ACTTAB (sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammer, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  int stateno;       /* The state-number */
  int major;         /* The major token value.  This is the code
                     ** number for the token at this stack level */
  YYMINORTYPE minor; /* The user-supplied minor token value.  This
                     ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
  int yyerrcnt;                 /* Shifts left before out of the error */
  xx_ARG_SDECL                /* A place to hold %extra_argument */
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void xx_Trace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *yyTokenName[] = { 
  "$",             "PUBLIC",        "PROTECTED",     "STATIC",      
  "PRIVATE",       "SCOPED",        "COMMA",         "AND",         
  "OR",            "EQUALS",        "IDENTICAL",     "LESS",        
  "GREATER",       "LESSEQUAL",     "GREATEREQUAL",  "NOTIDENTICAL",
  "NOTEQUALS",     "ADD",           "SUB",           "CONCAT",      
  "ISSET",         "FETCH",         "TYPEOF",        "INSTANCEOF",  
  "NEW",           "NOT",           "PARENTHESES_CLOSE",  "NAMESPACE",   
  "IDENTIFIER",    "DOTCOMMA",      "CLASS",         "BRACKET_OPEN",
  "BRACKET_CLOSE",  "EXTENDS",       "ABSTRACT",      "COMMENT",     
  "ASSIGN",        "CONST",         "CONSTANT",      "FUNCTION",    
  "PARENTHESES_OPEN",  "INLINE",        "FINAL",         "TYPE_INTEGER",
  "TYPE_DOUBLE",   "TYPE_BOOL",     "TYPE_STRING",   "TYPE_ARRAY",  
  "TYPE_VAR",      "BREAK",         "CONTINUE",      "IF",          
  "ELSE",          "SWITCH",        "CASE",          "COLON",       
  "DEFAULT",       "LOOP",          "WHILE",         "DO",          
  "FOR",           "IN",            "REVERSE",       "LET",         
  "ADDASSIGN",     "SUBASSIGN",     "MULASSIGN",     "DIVASSIGN",   
  "CONCATASSIGN",  "ARROW",         "SBRACKET_OPEN",  "SBRACKET_CLOSE",
  "DOUBLECOLON",   "INCR",          "DECR",          "ECHO",        
  "RETURN",        "UNSET",         "INTEGER",       "STRING",      
  "THROW",         "TYPE_UINTEGER",  "TYPE_CHAR",     "TYPE_UCHAR",  
  "TYPE_LONG",     "TYPE_ULONG",    "SCHAR",         "DOUBLE",      
  "NULL",          "TRUE",          "FALSE",         "error",       
  "program",       "xx_language",   "xx_top_statement_list",  "xx_top_statement",
  "xx_namespace_def",  "xx_class_def",  "xx_comment",    "xx_class_definition",
  "xx_class_properties_definition",  "xx_class_consts_definition",  "xx_class_methods_definition",  "xx_class_property_definition",
  "xx_visibility_list",  "xx_literal_expr",  "xx_class_const_definition",  "xx_class_method_definition",
  "xx_parameter_list",  "xx_statement_list",  "xx_visibility",  "xx_parameter",
  "xx_parameter_type",  "xx_parameter_cast",  "xx_statement",  "xx_let_statement",
  "xx_if_statement",  "xx_loop_statement",  "xx_echo_statement",  "xx_return_statement",
  "xx_mcall_statement",  "xx_scall_statement",  "xx_unset_statement",  "xx_throw_statement",
  "xx_declare_statement",  "xx_break_statement",  "xx_continue_statement",  "xx_while_statement",
  "xx_do_while_statement",  "xx_switch_statement",  "xx_for_statement",  "xx_eval_expr",
  "xx_case_clauses",  "xx_case_clause",  "xx_common_expr",  "xx_let_assignments",
  "xx_let_assignment",  "xx_assignment_operator",  "xx_assign_expr",  "xx_index_expr",
  "xx_echo_expressions",  "xx_echo_expression",  "xx_mcall_expr",  "xx_scall_expr",
  "xx_call_parameters",  "xx_declare_variable_list",  "xx_declare_variable",  "xx_isset_expr",
  "xx_array_list",  "xx_call_parameter",  "xx_array_item",  "xx_array_key",
  "xx_array_value",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *yyRuleName[] = {
 /*   0 */ "program ::= xx_language",
 /*   1 */ "xx_language ::= xx_top_statement_list",
 /*   2 */ "xx_top_statement_list ::= xx_top_statement_list xx_top_statement",
 /*   3 */ "xx_top_statement_list ::= xx_top_statement",
 /*   4 */ "xx_top_statement ::= xx_namespace_def",
 /*   5 */ "xx_top_statement ::= xx_class_def",
 /*   6 */ "xx_top_statement ::= xx_comment",
 /*   7 */ "xx_namespace_def ::= NAMESPACE IDENTIFIER DOTCOMMA",
 /*   8 */ "xx_class_def ::= CLASS IDENTIFIER BRACKET_OPEN xx_class_definition BRACKET_CLOSE",
 /*   9 */ "xx_class_def ::= CLASS IDENTIFIER EXTENDS IDENTIFIER BRACKET_OPEN xx_class_definition BRACKET_CLOSE",
 /*  10 */ "xx_class_def ::= ABSTRACT CLASS IDENTIFIER BRACKET_OPEN xx_class_definition BRACKET_CLOSE",
 /*  11 */ "xx_class_def ::= ABSTRACT CLASS IDENTIFIER EXTENDS IDENTIFIER BRACKET_OPEN xx_class_definition BRACKET_CLOSE",
 /*  12 */ "xx_class_def ::= CLASS IDENTIFIER BRACKET_OPEN BRACKET_CLOSE",
 /*  13 */ "xx_class_def ::= CLASS IDENTIFIER EXTENDS IDENTIFIER BRACKET_OPEN BRACKET_CLOSE",
 /*  14 */ "xx_class_def ::= ABSTRACT CLASS IDENTIFIER BRACKET_OPEN BRACKET_CLOSE",
 /*  15 */ "xx_class_def ::= ABSTRACT CLASS IDENTIFIER EXTENDS IDENTIFIER BRACKET_OPEN BRACKET_CLOSE",
 /*  16 */ "xx_class_definition ::= xx_class_properties_definition",
 /*  17 */ "xx_class_definition ::= xx_class_consts_definition",
 /*  18 */ "xx_class_definition ::= xx_class_methods_definition",
 /*  19 */ "xx_class_definition ::= xx_class_properties_definition xx_class_methods_definition",
 /*  20 */ "xx_class_definition ::= xx_class_properties_definition xx_class_consts_definition",
 /*  21 */ "xx_class_definition ::= xx_class_consts_definition xx_class_properties_definition",
 /*  22 */ "xx_class_definition ::= xx_class_consts_definition xx_class_methods_definition",
 /*  23 */ "xx_class_definition ::= xx_class_properties_definition xx_class_consts_definition xx_class_methods_definition",
 /*  24 */ "xx_class_definition ::= xx_class_consts_definition xx_class_properties_definition xx_class_methods_definition",
 /*  25 */ "xx_class_properties_definition ::= xx_class_properties_definition xx_class_property_definition",
 /*  26 */ "xx_class_properties_definition ::= xx_class_property_definition",
 /*  27 */ "xx_class_property_definition ::= COMMENT xx_visibility_list IDENTIFIER DOTCOMMA",
 /*  28 */ "xx_class_property_definition ::= xx_visibility_list IDENTIFIER DOTCOMMA",
 /*  29 */ "xx_class_property_definition ::= COMMENT xx_visibility_list IDENTIFIER ASSIGN xx_literal_expr DOTCOMMA",
 /*  30 */ "xx_class_property_definition ::= xx_visibility_list IDENTIFIER ASSIGN xx_literal_expr DOTCOMMA",
 /*  31 */ "xx_class_consts_definition ::= xx_class_consts_definition xx_class_const_definition",
 /*  32 */ "xx_class_consts_definition ::= xx_class_const_definition",
 /*  33 */ "xx_class_methods_definition ::= xx_class_methods_definition xx_class_method_definition",
 /*  34 */ "xx_class_methods_definition ::= xx_class_method_definition",
 /*  35 */ "xx_class_const_definition ::= COMMENT CONST CONSTANT ASSIGN xx_literal_expr DOTCOMMA",
 /*  36 */ "xx_class_const_definition ::= CONST CONSTANT ASSIGN xx_literal_expr DOTCOMMA",
 /*  37 */ "xx_class_method_definition ::= xx_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE BRACKET_OPEN BRACKET_CLOSE",
 /*  38 */ "xx_class_method_definition ::= xx_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN xx_parameter_list PARENTHESES_CLOSE BRACKET_OPEN BRACKET_CLOSE",
 /*  39 */ "xx_class_method_definition ::= xx_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /*  40 */ "xx_class_method_definition ::= xx_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN xx_parameter_list PARENTHESES_CLOSE BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /*  41 */ "xx_class_method_definition ::= COMMENT xx_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE BRACKET_OPEN BRACKET_CLOSE",
 /*  42 */ "xx_class_method_definition ::= COMMENT xx_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN xx_parameter_list PARENTHESES_CLOSE BRACKET_OPEN BRACKET_CLOSE",
 /*  43 */ "xx_class_method_definition ::= COMMENT xx_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /*  44 */ "xx_class_method_definition ::= COMMENT xx_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN xx_parameter_list PARENTHESES_CLOSE BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /*  45 */ "xx_visibility_list ::= xx_visibility_list xx_visibility",
 /*  46 */ "xx_visibility_list ::= xx_visibility",
 /*  47 */ "xx_visibility ::= PUBLIC",
 /*  48 */ "xx_visibility ::= PROTECTED",
 /*  49 */ "xx_visibility ::= PRIVATE",
 /*  50 */ "xx_visibility ::= STATIC",
 /*  51 */ "xx_visibility ::= SCOPED",
 /*  52 */ "xx_visibility ::= INLINE",
 /*  53 */ "xx_visibility ::= ABSTRACT",
 /*  54 */ "xx_visibility ::= FINAL",
 /*  55 */ "xx_parameter_list ::= xx_parameter_list COMMA xx_parameter",
 /*  56 */ "xx_parameter_list ::= xx_parameter",
 /*  57 */ "xx_parameter ::= IDENTIFIER",
 /*  58 */ "xx_parameter ::= xx_parameter_type IDENTIFIER",
 /*  59 */ "xx_parameter ::= xx_parameter_cast IDENTIFIER",
 /*  60 */ "xx_parameter ::= IDENTIFIER ASSIGN xx_literal_expr",
 /*  61 */ "xx_parameter ::= xx_parameter_type IDENTIFIER ASSIGN xx_literal_expr",
 /*  62 */ "xx_parameter ::= xx_parameter_cast IDENTIFIER ASSIGN xx_literal_expr",
 /*  63 */ "xx_parameter_cast ::= LESS IDENTIFIER GREATER",
 /*  64 */ "xx_parameter_type ::= TYPE_INTEGER",
 /*  65 */ "xx_parameter_type ::= TYPE_DOUBLE",
 /*  66 */ "xx_parameter_type ::= TYPE_BOOL",
 /*  67 */ "xx_parameter_type ::= TYPE_STRING",
 /*  68 */ "xx_parameter_type ::= TYPE_ARRAY",
 /*  69 */ "xx_parameter_type ::= TYPE_VAR",
 /*  70 */ "xx_statement_list ::= xx_statement_list xx_statement",
 /*  71 */ "xx_statement_list ::= xx_statement",
 /*  72 */ "xx_statement ::= xx_let_statement",
 /*  73 */ "xx_statement ::= xx_if_statement",
 /*  74 */ "xx_statement ::= xx_loop_statement",
 /*  75 */ "xx_statement ::= xx_echo_statement",
 /*  76 */ "xx_statement ::= xx_return_statement",
 /*  77 */ "xx_statement ::= xx_mcall_statement",
 /*  78 */ "xx_statement ::= xx_scall_statement",
 /*  79 */ "xx_statement ::= xx_unset_statement",
 /*  80 */ "xx_statement ::= xx_throw_statement",
 /*  81 */ "xx_statement ::= xx_declare_statement",
 /*  82 */ "xx_statement ::= xx_break_statement",
 /*  83 */ "xx_statement ::= xx_continue_statement",
 /*  84 */ "xx_statement ::= xx_while_statement",
 /*  85 */ "xx_statement ::= xx_do_while_statement",
 /*  86 */ "xx_statement ::= xx_switch_statement",
 /*  87 */ "xx_statement ::= xx_for_statement",
 /*  88 */ "xx_statement ::= xx_comment",
 /*  89 */ "xx_break_statement ::= BREAK DOTCOMMA",
 /*  90 */ "xx_continue_statement ::= CONTINUE DOTCOMMA",
 /*  91 */ "xx_if_statement ::= IF xx_eval_expr BRACKET_OPEN BRACKET_CLOSE",
 /*  92 */ "xx_if_statement ::= IF xx_eval_expr BRACKET_OPEN BRACKET_CLOSE ELSE BRACKET_OPEN BRACKET_CLOSE",
 /*  93 */ "xx_if_statement ::= IF xx_eval_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /*  94 */ "xx_if_statement ::= IF xx_eval_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE ELSE BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /*  95 */ "xx_if_statement ::= IF xx_eval_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE ELSE BRACKET_OPEN BRACKET_CLOSE",
 /*  96 */ "xx_if_statement ::= IF xx_eval_expr BRACKET_OPEN BRACKET_CLOSE ELSE BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /*  97 */ "xx_switch_statement ::= SWITCH xx_eval_expr BRACKET_OPEN BRACKET_CLOSE",
 /*  98 */ "xx_switch_statement ::= SWITCH xx_eval_expr BRACKET_OPEN xx_case_clauses BRACKET_CLOSE",
 /*  99 */ "xx_case_clauses ::= xx_case_clauses xx_case_clause",
 /* 100 */ "xx_case_clauses ::= xx_case_clause",
 /* 101 */ "xx_case_clause ::= CASE xx_literal_expr COLON xx_statement_list",
 /* 102 */ "xx_case_clause ::= DEFAULT COLON xx_statement_list",
 /* 103 */ "xx_loop_statement ::= LOOP BRACKET_OPEN BRACKET_CLOSE",
 /* 104 */ "xx_loop_statement ::= LOOP BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 105 */ "xx_while_statement ::= WHILE xx_eval_expr BRACKET_OPEN BRACKET_CLOSE",
 /* 106 */ "xx_while_statement ::= WHILE xx_eval_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 107 */ "xx_do_while_statement ::= DO BRACKET_OPEN BRACKET_CLOSE WHILE xx_eval_expr DOTCOMMA",
 /* 108 */ "xx_do_while_statement ::= DO BRACKET_OPEN xx_statement_list BRACKET_CLOSE WHILE xx_eval_expr DOTCOMMA",
 /* 109 */ "xx_for_statement ::= FOR IDENTIFIER IN xx_common_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 110 */ "xx_for_statement ::= FOR IDENTIFIER IN REVERSE xx_common_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 111 */ "xx_for_statement ::= FOR IDENTIFIER COMMA IDENTIFIER IN xx_common_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 112 */ "xx_for_statement ::= FOR IDENTIFIER COMMA IDENTIFIER IN REVERSE xx_common_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 113 */ "xx_let_statement ::= LET xx_let_assignments DOTCOMMA",
 /* 114 */ "xx_let_assignments ::= xx_let_assignments COMMA xx_let_assignment",
 /* 115 */ "xx_let_assignments ::= xx_let_assignment",
 /* 116 */ "xx_assignment_operator ::= ASSIGN",
 /* 117 */ "xx_assignment_operator ::= ADDASSIGN",
 /* 118 */ "xx_assignment_operator ::= SUBASSIGN",
 /* 119 */ "xx_assignment_operator ::= MULASSIGN",
 /* 120 */ "xx_assignment_operator ::= DIVASSIGN",
 /* 121 */ "xx_assignment_operator ::= CONCATASSIGN",
 /* 122 */ "xx_let_assignment ::= IDENTIFIER xx_assignment_operator xx_assign_expr",
 /* 123 */ "xx_let_assignment ::= IDENTIFIER ARROW IDENTIFIER xx_assignment_operator xx_assign_expr",
 /* 124 */ "xx_let_assignment ::= IDENTIFIER ARROW IDENTIFIER SBRACKET_OPEN SBRACKET_CLOSE xx_assignment_operator xx_assign_expr",
 /* 125 */ "xx_let_assignment ::= IDENTIFIER ARROW IDENTIFIER SBRACKET_OPEN xx_index_expr SBRACKET_CLOSE xx_assignment_operator xx_assign_expr",
 /* 126 */ "xx_let_assignment ::= IDENTIFIER DOUBLECOLON IDENTIFIER xx_assignment_operator xx_assign_expr",
 /* 127 */ "xx_let_assignment ::= IDENTIFIER DOUBLECOLON IDENTIFIER SBRACKET_OPEN SBRACKET_CLOSE xx_assignment_operator xx_assign_expr",
 /* 128 */ "xx_let_assignment ::= IDENTIFIER DOUBLECOLON IDENTIFIER SBRACKET_OPEN xx_index_expr SBRACKET_CLOSE xx_assignment_operator xx_assign_expr",
 /* 129 */ "xx_let_assignment ::= IDENTIFIER SBRACKET_OPEN SBRACKET_CLOSE xx_assignment_operator xx_assign_expr",
 /* 130 */ "xx_let_assignment ::= IDENTIFIER SBRACKET_OPEN xx_index_expr SBRACKET_CLOSE xx_assignment_operator xx_assign_expr",
 /* 131 */ "xx_let_assignment ::= IDENTIFIER INCR",
 /* 132 */ "xx_let_assignment ::= IDENTIFIER DECR",
 /* 133 */ "xx_index_expr ::= xx_common_expr",
 /* 134 */ "xx_echo_statement ::= ECHO xx_echo_expressions DOTCOMMA",
 /* 135 */ "xx_echo_expressions ::= xx_echo_expressions COMMA xx_echo_expression",
 /* 136 */ "xx_echo_expressions ::= xx_echo_expression",
 /* 137 */ "xx_echo_expression ::= xx_common_expr",
 /* 138 */ "xx_mcall_statement ::= xx_mcall_expr DOTCOMMA",
 /* 139 */ "xx_scall_statement ::= xx_scall_expr DOTCOMMA",
 /* 140 */ "xx_return_statement ::= RETURN xx_common_expr DOTCOMMA",
 /* 141 */ "xx_unset_statement ::= UNSET IDENTIFIER ARROW IDENTIFIER DOTCOMMA",
 /* 142 */ "xx_unset_statement ::= UNSET IDENTIFIER SBRACKET_OPEN IDENTIFIER SBRACKET_CLOSE DOTCOMMA",
 /* 143 */ "xx_unset_statement ::= UNSET IDENTIFIER SBRACKET_OPEN INTEGER SBRACKET_CLOSE DOTCOMMA",
 /* 144 */ "xx_unset_statement ::= UNSET IDENTIFIER SBRACKET_OPEN STRING SBRACKET_CLOSE DOTCOMMA",
 /* 145 */ "xx_throw_statement ::= THROW IDENTIFIER DOTCOMMA",
 /* 146 */ "xx_throw_statement ::= THROW NEW IDENTIFIER PARENTHESES_OPEN xx_call_parameters PARENTHESES_CLOSE DOTCOMMA",
 /* 147 */ "xx_declare_statement ::= TYPE_INTEGER xx_declare_variable_list DOTCOMMA",
 /* 148 */ "xx_declare_statement ::= TYPE_UINTEGER xx_declare_variable_list DOTCOMMA",
 /* 149 */ "xx_declare_statement ::= TYPE_CHAR xx_declare_variable_list DOTCOMMA",
 /* 150 */ "xx_declare_statement ::= TYPE_UCHAR xx_declare_variable_list DOTCOMMA",
 /* 151 */ "xx_declare_statement ::= TYPE_LONG xx_declare_variable_list DOTCOMMA",
 /* 152 */ "xx_declare_statement ::= TYPE_ULONG xx_declare_variable_list DOTCOMMA",
 /* 153 */ "xx_declare_statement ::= TYPE_DOUBLE xx_declare_variable_list DOTCOMMA",
 /* 154 */ "xx_declare_statement ::= TYPE_STRING xx_declare_variable_list DOTCOMMA",
 /* 155 */ "xx_declare_statement ::= TYPE_BOOL xx_declare_variable_list DOTCOMMA",
 /* 156 */ "xx_declare_statement ::= TYPE_VAR xx_declare_variable_list DOTCOMMA",
 /* 157 */ "xx_declare_variable_list ::= xx_declare_variable_list COMMA xx_declare_variable",
 /* 158 */ "xx_declare_variable_list ::= xx_declare_variable",
 /* 159 */ "xx_declare_variable ::= IDENTIFIER",
 /* 160 */ "xx_declare_variable ::= IDENTIFIER ASSIGN xx_literal_expr",
 /* 161 */ "xx_assign_expr ::= xx_common_expr",
 /* 162 */ "xx_common_expr ::= NOT xx_common_expr",
 /* 163 */ "xx_common_expr ::= ISSET xx_isset_expr",
 /* 164 */ "xx_common_expr ::= xx_common_expr EQUALS xx_common_expr",
 /* 165 */ "xx_common_expr ::= xx_common_expr NOTEQUALS xx_common_expr",
 /* 166 */ "xx_common_expr ::= xx_common_expr IDENTICAL xx_common_expr",
 /* 167 */ "xx_common_expr ::= xx_common_expr NOTIDENTICAL xx_common_expr",
 /* 168 */ "xx_common_expr ::= xx_common_expr LESS xx_common_expr",
 /* 169 */ "xx_common_expr ::= xx_common_expr GREATER xx_common_expr",
 /* 170 */ "xx_common_expr ::= xx_common_expr LESSEQUAL xx_common_expr",
 /* 171 */ "xx_common_expr ::= xx_common_expr GREATEREQUAL xx_common_expr",
 /* 172 */ "xx_common_expr ::= PARENTHESES_OPEN xx_common_expr PARENTHESES_CLOSE",
 /* 173 */ "xx_common_expr ::= PARENTHESES_OPEN xx_parameter_type PARENTHESES_CLOSE xx_common_expr",
 /* 174 */ "xx_common_expr ::= IDENTIFIER ARROW IDENTIFIER",
 /* 175 */ "xx_common_expr ::= IDENTIFIER DOUBLECOLON IDENTIFIER",
 /* 176 */ "xx_common_expr ::= IDENTIFIER DOUBLECOLON CONSTANT",
 /* 177 */ "xx_common_expr ::= IDENTIFIER SBRACKET_OPEN IDENTIFIER SBRACKET_CLOSE",
 /* 178 */ "xx_common_expr ::= IDENTIFIER SBRACKET_OPEN INTEGER SBRACKET_CLOSE",
 /* 179 */ "xx_common_expr ::= IDENTIFIER SBRACKET_OPEN STRING SBRACKET_CLOSE",
 /* 180 */ "xx_common_expr ::= xx_common_expr ADD xx_common_expr",
 /* 181 */ "xx_common_expr ::= xx_common_expr SUB xx_common_expr",
 /* 182 */ "xx_common_expr ::= xx_common_expr CONCAT xx_common_expr",
 /* 183 */ "xx_common_expr ::= xx_common_expr AND xx_common_expr",
 /* 184 */ "xx_common_expr ::= xx_common_expr OR xx_common_expr",
 /* 185 */ "xx_common_expr ::= INSTANCEOF xx_common_expr",
 /* 186 */ "xx_common_expr ::= FETCH IDENTIFIER COMMA xx_isset_expr",
 /* 187 */ "xx_common_expr ::= TYPEOF xx_common_expr",
 /* 188 */ "xx_common_expr ::= CONSTANT",
 /* 189 */ "xx_common_expr ::= IDENTIFIER",
 /* 190 */ "xx_common_expr ::= INTEGER",
 /* 191 */ "xx_common_expr ::= STRING",
 /* 192 */ "xx_common_expr ::= SCHAR",
 /* 193 */ "xx_common_expr ::= DOUBLE",
 /* 194 */ "xx_common_expr ::= NULL",
 /* 195 */ "xx_common_expr ::= TRUE",
 /* 196 */ "xx_common_expr ::= FALSE",
 /* 197 */ "xx_common_expr ::= SBRACKET_OPEN SBRACKET_CLOSE",
 /* 198 */ "xx_common_expr ::= SBRACKET_OPEN xx_array_list SBRACKET_CLOSE",
 /* 199 */ "xx_common_expr ::= NEW IDENTIFIER",
 /* 200 */ "xx_common_expr ::= NEW IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE",
 /* 201 */ "xx_common_expr ::= NEW IDENTIFIER PARENTHESES_OPEN xx_call_parameters PARENTHESES_CLOSE",
 /* 202 */ "xx_common_expr ::= IDENTIFIER PARENTHESES_OPEN xx_call_parameters PARENTHESES_CLOSE",
 /* 203 */ "xx_common_expr ::= IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE",
 /* 204 */ "xx_common_expr ::= xx_mcall_expr",
 /* 205 */ "xx_common_expr ::= xx_scall_expr",
 /* 206 */ "xx_scall_expr ::= IDENTIFIER DOUBLECOLON IDENTIFIER PARENTHESES_OPEN xx_call_parameters PARENTHESES_CLOSE",
 /* 207 */ "xx_scall_expr ::= IDENTIFIER DOUBLECOLON IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE",
 /* 208 */ "xx_mcall_expr ::= IDENTIFIER ARROW IDENTIFIER PARENTHESES_OPEN xx_call_parameters PARENTHESES_CLOSE",
 /* 209 */ "xx_mcall_expr ::= IDENTIFIER ARROW IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE",
 /* 210 */ "xx_call_parameters ::= xx_call_parameters COMMA xx_call_parameter",
 /* 211 */ "xx_call_parameters ::= xx_call_parameter",
 /* 212 */ "xx_call_parameter ::= xx_common_expr",
 /* 213 */ "xx_array_list ::= xx_array_list COMMA xx_array_item",
 /* 214 */ "xx_array_list ::= xx_array_item",
 /* 215 */ "xx_array_item ::= xx_array_key COLON xx_array_value",
 /* 216 */ "xx_array_item ::= xx_array_value",
 /* 217 */ "xx_array_key ::= IDENTIFIER",
 /* 218 */ "xx_array_key ::= STRING",
 /* 219 */ "xx_array_key ::= INTEGER",
 /* 220 */ "xx_array_value ::= xx_literal_expr",
 /* 221 */ "xx_array_value ::= IDENTIFIER",
 /* 222 */ "xx_literal_expr ::= INTEGER",
 /* 223 */ "xx_literal_expr ::= STRING",
 /* 224 */ "xx_literal_expr ::= DOUBLE",
 /* 225 */ "xx_literal_expr ::= NULL",
 /* 226 */ "xx_literal_expr ::= FALSE",
 /* 227 */ "xx_literal_expr ::= TRUE",
 /* 228 */ "xx_isset_expr ::= IDENTIFIER SBRACKET_OPEN IDENTIFIER SBRACKET_CLOSE",
 /* 229 */ "xx_isset_expr ::= IDENTIFIER SBRACKET_OPEN INTEGER SBRACKET_CLOSE",
 /* 230 */ "xx_isset_expr ::= IDENTIFIER SBRACKET_OPEN STRING SBRACKET_CLOSE",
 /* 231 */ "xx_eval_expr ::= xx_common_expr",
 /* 232 */ "xx_comment ::= COMMENT",
};
#endif /* NDEBUG */

/*
** This function returns the symbolic name associated with a token
** value.
*/
const char *xx_TokenName(int tokenType){
#ifndef NDEBUG
  if( tokenType>0 && tokenType<(sizeof(yyTokenName)/sizeof(yyTokenName[0])) ){
    return yyTokenName[tokenType];
  }else{
    return "Unknown";
  }
#else
  return "";
#endif
}

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to xx_ and xx_Free.
*/
void *xx_Alloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(YYCODETYPE yymajor, YYMINORTYPE *yypminor){
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
    case 80:
    case 81:
    case 82:
    case 83:
    case 84:
    case 85:
    case 86:
    case 87:
    case 88:
    case 89:
    case 90:
// 835 "parser.lemon"
{
	/*if ((yypminor->yy0)) {
		if ((yypminor->yy0)->free_flag) {
			efree((yypminor->yy0)->token);
		}
		efree((yypminor->yy0));
	}*/
}
// 2150 "parser.c"
      break;
    case 93:
// 848 "parser.lemon"
{ json_object_put((yypminor->yy115)); }
// 2155 "parser.c"
      break;
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor( yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from xx_Alloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void xx_Free(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
  (*freeProc)((void*)pParser);
}

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  int iLookAhead            /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  /* if( pParser->yyidx<0 ) return YY_NO_ACTION;  */
  i = yy_shift_ofst[stateno];
  if( i==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  if( iLookAhead==YYNOCODE ){
    return YY_NO_ACTION;
  }
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
#ifdef YYFALLBACK
    int iFallback;            /* Fallback token */
    if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
           && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
           yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
      }
#endif
      return yy_find_shift_action(pParser, iFallback);
    }
#endif
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  yyParser *pParser,        /* The parser */
  int iLookAhead            /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  i = yy_reduce_ofst[stateno];
  if( i==YY_REDUCE_USE_DFLT ){
    return yy_default[stateno];
  }
  if( iLookAhead==YYNOCODE ){
    return YY_NO_ACTION;
  }
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer ot the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
  if( yypParser->yyidx>=YYSTACKDEPTH ){
     xx_ARG_FETCH;
     yypParser->yyidx--;
#ifndef NDEBUG
     if( yyTraceFILE ){
       fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
     }
#endif
     while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
     /* Here code is inserted which will execute if the parser
     ** stack every overflows */
     xx_ARG_STORE; /* Suppress warning about unused %extra_argument var */
     return;
  }
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = yyNewState;
  yytos->major = yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 92, 1 },
  { 93, 1 },
  { 94, 2 },
  { 94, 1 },
  { 95, 1 },
  { 95, 1 },
  { 95, 1 },
  { 96, 3 },
  { 97, 5 },
  { 97, 7 },
  { 97, 6 },
  { 97, 8 },
  { 97, 4 },
  { 97, 6 },
  { 97, 5 },
  { 97, 7 },
  { 99, 1 },
  { 99, 1 },
  { 99, 1 },
  { 99, 2 },
  { 99, 2 },
  { 99, 2 },
  { 99, 2 },
  { 99, 3 },
  { 99, 3 },
  { 100, 2 },
  { 100, 1 },
  { 103, 4 },
  { 103, 3 },
  { 103, 6 },
  { 103, 5 },
  { 101, 2 },
  { 101, 1 },
  { 102, 2 },
  { 102, 1 },
  { 106, 6 },
  { 106, 5 },
  { 107, 7 },
  { 107, 8 },
  { 107, 8 },
  { 107, 9 },
  { 107, 8 },
  { 107, 9 },
  { 107, 9 },
  { 107, 10 },
  { 104, 2 },
  { 104, 1 },
  { 110, 1 },
  { 110, 1 },
  { 110, 1 },
  { 110, 1 },
  { 110, 1 },
  { 110, 1 },
  { 110, 1 },
  { 110, 1 },
  { 108, 3 },
  { 108, 1 },
  { 111, 1 },
  { 111, 2 },
  { 111, 2 },
  { 111, 3 },
  { 111, 4 },
  { 111, 4 },
  { 113, 3 },
  { 112, 1 },
  { 112, 1 },
  { 112, 1 },
  { 112, 1 },
  { 112, 1 },
  { 112, 1 },
  { 109, 2 },
  { 109, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 125, 2 },
  { 126, 2 },
  { 116, 4 },
  { 116, 7 },
  { 116, 5 },
  { 116, 9 },
  { 116, 8 },
  { 116, 8 },
  { 129, 4 },
  { 129, 5 },
  { 132, 2 },
  { 132, 1 },
  { 133, 4 },
  { 133, 3 },
  { 117, 3 },
  { 117, 4 },
  { 127, 4 },
  { 127, 5 },
  { 128, 6 },
  { 128, 7 },
  { 130, 7 },
  { 130, 8 },
  { 130, 9 },
  { 130, 10 },
  { 115, 3 },
  { 135, 3 },
  { 135, 1 },
  { 137, 1 },
  { 137, 1 },
  { 137, 1 },
  { 137, 1 },
  { 137, 1 },
  { 137, 1 },
  { 136, 3 },
  { 136, 5 },
  { 136, 7 },
  { 136, 8 },
  { 136, 5 },
  { 136, 7 },
  { 136, 8 },
  { 136, 5 },
  { 136, 6 },
  { 136, 2 },
  { 136, 2 },
  { 139, 1 },
  { 118, 3 },
  { 140, 3 },
  { 140, 1 },
  { 141, 1 },
  { 120, 2 },
  { 121, 2 },
  { 119, 3 },
  { 122, 5 },
  { 122, 6 },
  { 122, 6 },
  { 122, 6 },
  { 123, 3 },
  { 123, 7 },
  { 124, 3 },
  { 124, 3 },
  { 124, 3 },
  { 124, 3 },
  { 124, 3 },
  { 124, 3 },
  { 124, 3 },
  { 124, 3 },
  { 124, 3 },
  { 124, 3 },
  { 145, 3 },
  { 145, 1 },
  { 146, 1 },
  { 146, 3 },
  { 138, 1 },
  { 134, 2 },
  { 134, 2 },
  { 134, 3 },
  { 134, 3 },
  { 134, 3 },
  { 134, 3 },
  { 134, 3 },
  { 134, 3 },
  { 134, 3 },
  { 134, 3 },
  { 134, 3 },
  { 134, 4 },
  { 134, 3 },
  { 134, 3 },
  { 134, 3 },
  { 134, 4 },
  { 134, 4 },
  { 134, 4 },
  { 134, 3 },
  { 134, 3 },
  { 134, 3 },
  { 134, 3 },
  { 134, 3 },
  { 134, 2 },
  { 134, 4 },
  { 134, 2 },
  { 134, 1 },
  { 134, 1 },
  { 134, 1 },
  { 134, 1 },
  { 134, 1 },
  { 134, 1 },
  { 134, 1 },
  { 134, 1 },
  { 134, 1 },
  { 134, 2 },
  { 134, 3 },
  { 134, 2 },
  { 134, 4 },
  { 134, 5 },
  { 134, 4 },
  { 134, 3 },
  { 134, 1 },
  { 134, 1 },
  { 143, 6 },
  { 143, 5 },
  { 142, 6 },
  { 142, 5 },
  { 144, 3 },
  { 144, 1 },
  { 149, 1 },
  { 148, 3 },
  { 148, 1 },
  { 150, 3 },
  { 150, 1 },
  { 151, 1 },
  { 151, 1 },
  { 151, 1 },
  { 152, 1 },
  { 152, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 147, 4 },
  { 147, 4 },
  { 147, 4 },
  { 131, 1 },
  { 98, 1 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  xx_ARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<sizeof(yyRuleName)/sizeof(yyRuleName[0]) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  // <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  // <lineno> <thisfile>
  **     break;
  */
      case 0:
// 844 "parser.lemon"
{
	status->ret = yymsp[0].minor.yy115;
}
// 2605 "parser.c"
        break;
      case 1:
      case 4:
      case 5:
      case 6:
      case 72:
      case 73:
      case 74:
      case 75:
      case 76:
      case 77:
      case 78:
      case 79:
      case 80:
      case 81:
      case 82:
      case 83:
      case 84:
      case 85:
      case 86:
      case 87:
      case 88:
      case 133:
      case 161:
      case 204:
      case 205:
      case 212:
      case 220:
      case 231:
// 850 "parser.lemon"
{
	yygotominor.yy115 = yymsp[0].minor.yy115;
}
// 2639 "parser.c"
        break;
      case 2:
      case 25:
      case 31:
      case 33:
      case 45:
      case 70:
      case 99:
// 854 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_list(yymsp[-1].minor.yy115, yymsp[0].minor.yy115);
}
// 2652 "parser.c"
        break;
      case 3:
      case 26:
      case 32:
      case 34:
      case 46:
      case 56:
      case 71:
      case 100:
      case 115:
      case 136:
      case 158:
      case 211:
      case 214:
// 858 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_list(NULL, yymsp[0].minor.yy115);
}
// 2671 "parser.c"
        break;
      case 7:
// 874 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_namespace(yymsp[-1].minor.yy0, status->scanner_state);
  yy_destructor(27,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 2680 "parser.c"
        break;
      case 8:
// 878 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class(yymsp[-3].minor.yy0, yymsp[-1].minor.yy115, 0, NULL, status->scanner_state);
  yy_destructor(30,&yymsp[-4].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2690 "parser.c"
        break;
      case 9:
// 882 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class(yymsp[-5].minor.yy0, yymsp[-1].minor.yy115, 0, yymsp[-3].minor.yy0, status->scanner_state);
  yy_destructor(30,&yymsp[-6].minor);
  yy_destructor(33,&yymsp[-4].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2701 "parser.c"
        break;
      case 10:
// 886 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class(yymsp[-3].minor.yy0, yymsp[-1].minor.yy115, 1, NULL, status->scanner_state);
  yy_destructor(34,&yymsp[-5].minor);
  yy_destructor(30,&yymsp[-4].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2712 "parser.c"
        break;
      case 11:
// 890 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class(yymsp[-5].minor.yy0, yymsp[-1].minor.yy115, 1, yymsp[-3].minor.yy0, status->scanner_state);
  yy_destructor(34,&yymsp[-7].minor);
  yy_destructor(30,&yymsp[-6].minor);
  yy_destructor(33,&yymsp[-4].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2724 "parser.c"
        break;
      case 12:
// 894 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class(yymsp[-2].minor.yy0, NULL, 0, NULL, status->scanner_state);
  yy_destructor(30,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2734 "parser.c"
        break;
      case 13:
// 898 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class(yymsp[-4].minor.yy0, NULL, 0, yymsp[-2].minor.yy0, status->scanner_state);
  yy_destructor(30,&yymsp[-5].minor);
  yy_destructor(33,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2745 "parser.c"
        break;
      case 14:
// 902 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class(yymsp[-2].minor.yy0, NULL, 1, NULL, status->scanner_state);
  yy_destructor(34,&yymsp[-4].minor);
  yy_destructor(30,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2756 "parser.c"
        break;
      case 15:
// 906 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class(yymsp[-4].minor.yy0, NULL, 1, yymsp[-2].minor.yy0, status->scanner_state);
  yy_destructor(34,&yymsp[-6].minor);
  yy_destructor(30,&yymsp[-5].minor);
  yy_destructor(33,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2768 "parser.c"
        break;
      case 16:
// 910 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_definition(yymsp[0].minor.yy115, NULL, NULL, status->scanner_state);
}
// 2775 "parser.c"
        break;
      case 17:
// 914 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_definition(NULL, NULL, yymsp[0].minor.yy115, status->scanner_state);
}
// 2782 "parser.c"
        break;
      case 18:
// 918 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_definition(NULL, yymsp[0].minor.yy115, NULL, status->scanner_state);
}
// 2789 "parser.c"
        break;
      case 19:
// 922 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_definition(yymsp[-1].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
}
// 2796 "parser.c"
        break;
      case 20:
// 926 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_definition(yymsp[-1].minor.yy115, NULL, yymsp[0].minor.yy115, status->scanner_state);
}
// 2803 "parser.c"
        break;
      case 21:
// 930 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_definition(yymsp[0].minor.yy115, NULL, yymsp[-1].minor.yy115, status->scanner_state);
}
// 2810 "parser.c"
        break;
      case 22:
// 934 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_definition(NULL, yymsp[0].minor.yy115, yymsp[-1].minor.yy115, status->scanner_state);
}
// 2817 "parser.c"
        break;
      case 23:
// 938 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_definition(yymsp[-2].minor.yy115, yymsp[0].minor.yy115, yymsp[-1].minor.yy115, status->scanner_state);
}
// 2824 "parser.c"
        break;
      case 24:
// 942 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_definition(yymsp[-1].minor.yy115, yymsp[0].minor.yy115, yymsp[-2].minor.yy115, status->scanner_state);
}
// 2831 "parser.c"
        break;
      case 27:
// 954 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_property(yymsp[-2].minor.yy115, yymsp[-1].minor.yy0, NULL, yymsp[-3].minor.yy0, status->scanner_state);
  yy_destructor(29,&yymsp[0].minor);
}
// 2839 "parser.c"
        break;
      case 28:
// 958 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_property(yymsp[-2].minor.yy115, yymsp[-1].minor.yy0, NULL, NULL, status->scanner_state);
  yy_destructor(29,&yymsp[0].minor);
}
// 2847 "parser.c"
        break;
      case 29:
// 962 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_property(yymsp[-4].minor.yy115, yymsp[-3].minor.yy0, yymsp[-1].minor.yy115, yymsp[-5].minor.yy0, status->scanner_state);
  yy_destructor(36,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 2856 "parser.c"
        break;
      case 30:
// 966 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_property(yymsp[-4].minor.yy115, yymsp[-3].minor.yy0, yymsp[-1].minor.yy115, NULL, status->scanner_state);
  yy_destructor(36,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 2865 "parser.c"
        break;
      case 35:
// 986 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_const(yymsp[-3].minor.yy0, yymsp[-1].minor.yy115, yymsp[-5].minor.yy0, status->scanner_state);
  yy_destructor(37,&yymsp[-4].minor);
  yy_destructor(36,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 2875 "parser.c"
        break;
      case 36:
// 990 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_const(yymsp[-3].minor.yy0, yymsp[-1].minor.yy115, NULL, status->scanner_state);
  yy_destructor(37,&yymsp[-4].minor);
  yy_destructor(36,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 2885 "parser.c"
        break;
      case 37:
// 994 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_method(yymsp[-6].minor.yy115, yymsp[-4].minor.yy0, NULL, NULL, NULL, status->scanner_state);
  yy_destructor(39,&yymsp[-5].minor);
  yy_destructor(40,&yymsp[-3].minor);
  yy_destructor(26,&yymsp[-2].minor);
  yy_destructor(31,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2897 "parser.c"
        break;
      case 38:
// 998 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_method(yymsp[-7].minor.yy115, yymsp[-5].minor.yy0, yymsp[-3].minor.yy115, NULL, NULL, status->scanner_state);
  yy_destructor(39,&yymsp[-6].minor);
  yy_destructor(40,&yymsp[-4].minor);
  yy_destructor(26,&yymsp[-2].minor);
  yy_destructor(31,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2909 "parser.c"
        break;
      case 39:
// 1002 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_method(yymsp[-7].minor.yy115, yymsp[-5].minor.yy0, NULL, yymsp[-1].minor.yy115, NULL, status->scanner_state);
  yy_destructor(39,&yymsp[-6].minor);
  yy_destructor(40,&yymsp[-4].minor);
  yy_destructor(26,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2921 "parser.c"
        break;
      case 40:
// 1006 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_method(yymsp[-8].minor.yy115, yymsp[-6].minor.yy0, yymsp[-4].minor.yy115, yymsp[-1].minor.yy115, NULL, status->scanner_state);
  yy_destructor(39,&yymsp[-7].minor);
  yy_destructor(40,&yymsp[-5].minor);
  yy_destructor(26,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2933 "parser.c"
        break;
      case 41:
// 1010 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_method(yymsp[-6].minor.yy115, yymsp[-4].minor.yy0, NULL, NULL, yymsp[-7].minor.yy0, status->scanner_state);
  yy_destructor(39,&yymsp[-5].minor);
  yy_destructor(40,&yymsp[-3].minor);
  yy_destructor(26,&yymsp[-2].minor);
  yy_destructor(31,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2945 "parser.c"
        break;
      case 42:
// 1014 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_method(yymsp[-7].minor.yy115, yymsp[-5].minor.yy0, yymsp[-3].minor.yy115, NULL, yymsp[-8].minor.yy0, status->scanner_state);
  yy_destructor(39,&yymsp[-6].minor);
  yy_destructor(40,&yymsp[-4].minor);
  yy_destructor(26,&yymsp[-2].minor);
  yy_destructor(31,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2957 "parser.c"
        break;
      case 43:
// 1018 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_method(yymsp[-7].minor.yy115, yymsp[-5].minor.yy0, NULL, yymsp[-1].minor.yy115, yymsp[-8].minor.yy0, status->scanner_state);
  yy_destructor(39,&yymsp[-6].minor);
  yy_destructor(40,&yymsp[-4].minor);
  yy_destructor(26,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2969 "parser.c"
        break;
      case 44:
// 1022 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_class_method(yymsp[-8].minor.yy115, yymsp[-6].minor.yy0, yymsp[-4].minor.yy115, yymsp[-1].minor.yy115, yymsp[-9].minor.yy0, status->scanner_state);
  yy_destructor(39,&yymsp[-7].minor);
  yy_destructor(40,&yymsp[-5].minor);
  yy_destructor(26,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 2981 "parser.c"
        break;
      case 47:
// 1034 "parser.lemon"
{
	yygotominor.yy115 = json_object_new_string("public");
  yy_destructor(1,&yymsp[0].minor);
}
// 2989 "parser.c"
        break;
      case 48:
// 1038 "parser.lemon"
{
	yygotominor.yy115 = json_object_new_string("protected");
  yy_destructor(2,&yymsp[0].minor);
}
// 2997 "parser.c"
        break;
      case 49:
// 1042 "parser.lemon"
{
	yygotominor.yy115 = json_object_new_string("private");
  yy_destructor(4,&yymsp[0].minor);
}
// 3005 "parser.c"
        break;
      case 50:
// 1046 "parser.lemon"
{
	yygotominor.yy115 = json_object_new_string("static");
  yy_destructor(3,&yymsp[0].minor);
}
// 3013 "parser.c"
        break;
      case 51:
// 1050 "parser.lemon"
{
	yygotominor.yy115 = json_object_new_string("scoped");
  yy_destructor(5,&yymsp[0].minor);
}
// 3021 "parser.c"
        break;
      case 52:
// 1054 "parser.lemon"
{
	yygotominor.yy115 = json_object_new_string("inline");
  yy_destructor(41,&yymsp[0].minor);
}
// 3029 "parser.c"
        break;
      case 53:
// 1058 "parser.lemon"
{
	yygotominor.yy115 = json_object_new_string("abstract");
  yy_destructor(34,&yymsp[0].minor);
}
// 3037 "parser.c"
        break;
      case 54:
// 1062 "parser.lemon"
{
	yygotominor.yy115 = json_object_new_string("final");
  yy_destructor(42,&yymsp[0].minor);
}
// 3045 "parser.c"
        break;
      case 55:
      case 114:
      case 135:
      case 157:
      case 210:
      case 213:
// 1066 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_list(yymsp[-2].minor.yy115, yymsp[0].minor.yy115);
  yy_destructor(6,&yymsp[-1].minor);
}
// 3058 "parser.c"
        break;
      case 57:
// 1074 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_parameter(NULL, NULL, yymsp[0].minor.yy0, NULL, status->scanner_state);
}
// 3065 "parser.c"
        break;
      case 58:
// 1078 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_parameter(yymsp[-1].minor.yy115, NULL, yymsp[0].minor.yy0, NULL, status->scanner_state);
}
// 3072 "parser.c"
        break;
      case 59:
// 1082 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_parameter(NULL, yymsp[-1].minor.yy115, yymsp[0].minor.yy0, NULL, status->scanner_state);
}
// 3079 "parser.c"
        break;
      case 60:
// 1086 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_parameter(NULL, NULL, yymsp[-2].minor.yy0, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(36,&yymsp[-1].minor);
}
// 3087 "parser.c"
        break;
      case 61:
// 1090 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_parameter(yymsp[-3].minor.yy115, NULL, yymsp[-2].minor.yy0, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(36,&yymsp[-1].minor);
}
// 3095 "parser.c"
        break;
      case 62:
// 1094 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_parameter(NULL, yymsp[-3].minor.yy115, yymsp[-2].minor.yy0, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(36,&yymsp[-1].minor);
}
// 3103 "parser.c"
        break;
      case 63:
// 1098 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_literal(XX_T_IDENTIFIER, yymsp[-1].minor.yy0, status->scanner_state);
  yy_destructor(11,&yymsp[-2].minor);
  yy_destructor(12,&yymsp[0].minor);
}
// 3112 "parser.c"
        break;
      case 64:
// 1102 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_type(XX_TYPE_INTEGER);
  yy_destructor(43,&yymsp[0].minor);
}
// 3120 "parser.c"
        break;
      case 65:
// 1106 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_type(XX_TYPE_DOUBLE);
  yy_destructor(44,&yymsp[0].minor);
}
// 3128 "parser.c"
        break;
      case 66:
// 1110 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_type(XX_TYPE_BOOL);
  yy_destructor(45,&yymsp[0].minor);
}
// 3136 "parser.c"
        break;
      case 67:
// 1114 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_type(XX_TYPE_STRING);
  yy_destructor(46,&yymsp[0].minor);
}
// 3144 "parser.c"
        break;
      case 68:
// 1118 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_type(XX_TYPE_ARRAY);
  yy_destructor(47,&yymsp[0].minor);
}
// 3152 "parser.c"
        break;
      case 69:
// 1122 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_type(XX_TYPE_VAR);
  yy_destructor(48,&yymsp[0].minor);
}
// 3160 "parser.c"
        break;
      case 89:
// 1202 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_break_statement(status->scanner_state);
  yy_destructor(49,&yymsp[-1].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3169 "parser.c"
        break;
      case 90:
// 1206 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_continue_statement(status->scanner_state);
  yy_destructor(50,&yymsp[-1].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3178 "parser.c"
        break;
      case 91:
// 1210 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_if_statement(yymsp[-2].minor.yy115, NULL, NULL, status->scanner_state);
  yy_destructor(51,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3188 "parser.c"
        break;
      case 92:
// 1214 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_if_statement(yymsp[-5].minor.yy115, NULL, NULL, status->scanner_state);
  yy_destructor(51,&yymsp[-6].minor);
  yy_destructor(31,&yymsp[-4].minor);
  yy_destructor(32,&yymsp[-3].minor);
  yy_destructor(52,&yymsp[-2].minor);
  yy_destructor(31,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3201 "parser.c"
        break;
      case 93:
// 1218 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_if_statement(yymsp[-3].minor.yy115, yymsp[-1].minor.yy115, NULL, status->scanner_state);
  yy_destructor(51,&yymsp[-4].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3211 "parser.c"
        break;
      case 94:
// 1222 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_if_statement(yymsp[-7].minor.yy115, yymsp[-5].minor.yy115, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(51,&yymsp[-8].minor);
  yy_destructor(31,&yymsp[-6].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(52,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3224 "parser.c"
        break;
      case 95:
// 1226 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_if_statement(yymsp[-6].minor.yy115, yymsp[-4].minor.yy115, NULL, status->scanner_state);
  yy_destructor(51,&yymsp[-7].minor);
  yy_destructor(31,&yymsp[-5].minor);
  yy_destructor(32,&yymsp[-3].minor);
  yy_destructor(52,&yymsp[-2].minor);
  yy_destructor(31,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3237 "parser.c"
        break;
      case 96:
// 1230 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_if_statement(yymsp[-6].minor.yy115, NULL, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(51,&yymsp[-7].minor);
  yy_destructor(31,&yymsp[-5].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(52,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3250 "parser.c"
        break;
      case 97:
// 1234 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_switch_statement(yymsp[-2].minor.yy115, NULL, status->scanner_state);
  yy_destructor(53,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3260 "parser.c"
        break;
      case 98:
// 1238 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_switch_statement(yymsp[-3].minor.yy115, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(53,&yymsp[-4].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3270 "parser.c"
        break;
      case 101:
// 1250 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_case_clause(yymsp[-2].minor.yy115, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(54,&yymsp[-3].minor);
  yy_destructor(55,&yymsp[-1].minor);
}
// 3279 "parser.c"
        break;
      case 102:
// 1254 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_case_clause(NULL, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(56,&yymsp[-2].minor);
  yy_destructor(55,&yymsp[-1].minor);
}
// 3288 "parser.c"
        break;
      case 103:
// 1258 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_loop_statement(NULL, status->scanner_state);
  yy_destructor(57,&yymsp[-2].minor);
  yy_destructor(31,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3298 "parser.c"
        break;
      case 104:
// 1262 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_loop_statement(yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(57,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3308 "parser.c"
        break;
      case 105:
// 1266 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_while_statement(yymsp[-2].minor.yy115, NULL, status->scanner_state);
  yy_destructor(58,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3318 "parser.c"
        break;
      case 106:
// 1270 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_while_statement(yymsp[-3].minor.yy115, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(58,&yymsp[-4].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3328 "parser.c"
        break;
      case 107:
// 1274 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_do_while_statement(yymsp[-1].minor.yy115, NULL, status->scanner_state);
  yy_destructor(59,&yymsp[-5].minor);
  yy_destructor(31,&yymsp[-4].minor);
  yy_destructor(32,&yymsp[-3].minor);
  yy_destructor(58,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3340 "parser.c"
        break;
      case 108:
// 1278 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_do_while_statement(yymsp[-1].minor.yy115, yymsp[-4].minor.yy115, status->scanner_state);
  yy_destructor(59,&yymsp[-6].minor);
  yy_destructor(31,&yymsp[-5].minor);
  yy_destructor(32,&yymsp[-3].minor);
  yy_destructor(58,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3352 "parser.c"
        break;
      case 109:
// 1282 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_for_statement(yymsp[-3].minor.yy115, NULL, yymsp[-5].minor.yy0, 0, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(60,&yymsp[-6].minor);
  yy_destructor(61,&yymsp[-4].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3363 "parser.c"
        break;
      case 110:
// 1286 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_for_statement(yymsp[-3].minor.yy115, NULL, yymsp[-6].minor.yy0, 1, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(60,&yymsp[-7].minor);
  yy_destructor(61,&yymsp[-5].minor);
  yy_destructor(62,&yymsp[-4].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3375 "parser.c"
        break;
      case 111:
// 1290 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_for_statement(yymsp[-3].minor.yy115, yymsp[-7].minor.yy0, yymsp[-5].minor.yy0, 0, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(60,&yymsp[-8].minor);
  yy_destructor(6,&yymsp[-6].minor);
  yy_destructor(61,&yymsp[-4].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3387 "parser.c"
        break;
      case 112:
// 1294 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_for_statement(yymsp[-3].minor.yy115, yymsp[-8].minor.yy0, yymsp[-6].minor.yy0, 1, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(60,&yymsp[-9].minor);
  yy_destructor(6,&yymsp[-7].minor);
  yy_destructor(61,&yymsp[-5].minor);
  yy_destructor(62,&yymsp[-4].minor);
  yy_destructor(31,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
// 3400 "parser.c"
        break;
      case 113:
// 1298 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_let_statement(yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(63,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3409 "parser.c"
        break;
      case 116:
// 1311 "parser.lemon"
{
	yygotominor.yy115 = json_object_new_string("assign");
  yy_destructor(36,&yymsp[0].minor);
}
// 3417 "parser.c"
        break;
      case 117:
// 1316 "parser.lemon"
{
	yygotominor.yy115 = json_object_new_string("add-assign");
  yy_destructor(64,&yymsp[0].minor);
}
// 3425 "parser.c"
        break;
      case 118:
// 1321 "parser.lemon"
{
	yygotominor.yy115 = json_object_new_string("sub-assign");
  yy_destructor(65,&yymsp[0].minor);
}
// 3433 "parser.c"
        break;
      case 119:
// 1325 "parser.lemon"
{
	yygotominor.yy115 = json_object_new_string("mult-assign");
  yy_destructor(66,&yymsp[0].minor);
}
// 3441 "parser.c"
        break;
      case 120:
// 1329 "parser.lemon"
{
	yygotominor.yy115 = json_object_new_string("div-assign");
  yy_destructor(67,&yymsp[0].minor);
}
// 3449 "parser.c"
        break;
      case 121:
// 1333 "parser.lemon"
{
	yygotominor.yy115 = json_object_new_string("concat-assign");
  yy_destructor(68,&yymsp[0].minor);
}
// 3457 "parser.c"
        break;
      case 122:
// 1338 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_let_assignment("variable", yymsp[-1].minor.yy115, yymsp[-2].minor.yy0, NULL, NULL, yymsp[0].minor.yy115, status->scanner_state);
}
// 3464 "parser.c"
        break;
      case 123:
// 1343 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_let_assignment("object-property", yymsp[-1].minor.yy115, yymsp[-4].minor.yy0, yymsp[-2].minor.yy0, NULL, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(69,&yymsp[-3].minor);
}
// 3472 "parser.c"
        break;
      case 124:
// 1348 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_let_assignment("object-property-append", yymsp[-1].minor.yy115, yymsp[-6].minor.yy0, yymsp[-4].minor.yy0, NULL, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(69,&yymsp[-5].minor);
  yy_destructor(70,&yymsp[-3].minor);
  yy_destructor(71,&yymsp[-2].minor);
}
// 3482 "parser.c"
        break;
      case 125:
// 1353 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_let_assignment("object-property-array-index", yymsp[-1].minor.yy115, yymsp[-7].minor.yy0, yymsp[-5].minor.yy0, yymsp[-3].minor.yy115, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(69,&yymsp[-6].minor);
  yy_destructor(70,&yymsp[-4].minor);
  yy_destructor(71,&yymsp[-2].minor);
}
// 3492 "parser.c"
        break;
      case 126:
// 1358 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_let_assignment("static-property", yymsp[-1].minor.yy115, yymsp[-4].minor.yy0, yymsp[-2].minor.yy0, NULL, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(72,&yymsp[-3].minor);
}
// 3500 "parser.c"
        break;
      case 127:
// 1363 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_let_assignment("static-property-append", yymsp[-1].minor.yy115, yymsp[-6].minor.yy0, yymsp[-4].minor.yy0, NULL, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(72,&yymsp[-5].minor);
  yy_destructor(70,&yymsp[-3].minor);
  yy_destructor(71,&yymsp[-2].minor);
}
// 3510 "parser.c"
        break;
      case 128:
// 1368 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_let_assignment("static-property-array-index", yymsp[-1].minor.yy115, yymsp[-7].minor.yy0, yymsp[-5].minor.yy0, yymsp[-3].minor.yy115, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(72,&yymsp[-6].minor);
  yy_destructor(70,&yymsp[-4].minor);
  yy_destructor(71,&yymsp[-2].minor);
}
// 3520 "parser.c"
        break;
      case 129:
// 1373 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_let_assignment("variable-append", yymsp[-1].minor.yy115, yymsp[-4].minor.yy0, NULL, NULL, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(70,&yymsp[-3].minor);
  yy_destructor(71,&yymsp[-2].minor);
}
// 3529 "parser.c"
        break;
      case 130:
// 1378 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_let_assignment("array-index", yymsp[-1].minor.yy115, yymsp[-5].minor.yy0, NULL, yymsp[-3].minor.yy115, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(70,&yymsp[-4].minor);
  yy_destructor(71,&yymsp[-2].minor);
}
// 3538 "parser.c"
        break;
      case 131:
// 1383 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_let_assignment("incr", NULL, yymsp[-1].minor.yy0, NULL, NULL, NULL, status->scanner_state);
  yy_destructor(73,&yymsp[0].minor);
}
// 3546 "parser.c"
        break;
      case 132:
// 1388 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_let_assignment("decr", NULL, yymsp[-1].minor.yy0, NULL, NULL, NULL, status->scanner_state);
  yy_destructor(74,&yymsp[0].minor);
}
// 3554 "parser.c"
        break;
      case 134:
// 1396 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_echo_statement(yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(75,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3563 "parser.c"
        break;
      case 137:
// 1408 "parser.lemon"
{
	yygotominor.yy115 = yymsp[0].minor.yy115;;
}
// 3570 "parser.c"
        break;
      case 138:
// 1413 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_mcall_statement(yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(29,&yymsp[0].minor);
}
// 3578 "parser.c"
        break;
      case 139:
// 1418 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_scall_statement(yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(29,&yymsp[0].minor);
}
// 3586 "parser.c"
        break;
      case 140:
// 1423 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_return_statement(yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(76,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3595 "parser.c"
        break;
      case 141:
// 1428 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_unset_statement(
		xx_ret_literal(XX_T_IDENTIFIER, yymsp[-3].minor.yy0, status->scanner_state),
		xx_ret_literal(XX_T_IDENTIFIER, yymsp[-1].minor.yy0, status->scanner_state),
		status->scanner_state
	);
  yy_destructor(77,&yymsp[-4].minor);
  yy_destructor(69,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3609 "parser.c"
        break;
      case 142:
// 1437 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_unset_statement(
		xx_ret_literal(XX_T_IDENTIFIER, yymsp[-4].minor.yy0, status->scanner_state),
		xx_ret_literal(XX_T_IDENTIFIER, yymsp[-2].minor.yy0, status->scanner_state),
		status->scanner_state
	);
  yy_destructor(77,&yymsp[-5].minor);
  yy_destructor(70,&yymsp[-3].minor);
  yy_destructor(71,&yymsp[-1].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3624 "parser.c"
        break;
      case 143:
// 1446 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_unset_statement(
		xx_ret_literal(XX_T_IDENTIFIER, yymsp[-4].minor.yy0, status->scanner_state),
		xx_ret_literal(XX_T_INTEGER, yymsp[-2].minor.yy0, status->scanner_state),
		status->scanner_state
	);
  yy_destructor(77,&yymsp[-5].minor);
  yy_destructor(70,&yymsp[-3].minor);
  yy_destructor(71,&yymsp[-1].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3639 "parser.c"
        break;
      case 144:
// 1455 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_unset_statement(
		xx_ret_literal(XX_T_IDENTIFIER, yymsp[-4].minor.yy0, status->scanner_state),
		xx_ret_literal(XX_T_STRING, yymsp[-2].minor.yy0, status->scanner_state),
		status->scanner_state
	);
  yy_destructor(77,&yymsp[-5].minor);
  yy_destructor(70,&yymsp[-3].minor);
  yy_destructor(71,&yymsp[-1].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3654 "parser.c"
        break;
      case 145:
// 1464 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_throw_exception(yymsp[-1].minor.yy0, NULL, status->scanner_state);
  yy_destructor(80,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3663 "parser.c"
        break;
      case 146:
// 1469 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_throw_exception(yymsp[-4].minor.yy0, yymsp[-2].minor.yy115, status->scanner_state);
  yy_destructor(80,&yymsp[-6].minor);
  yy_destructor(24,&yymsp[-5].minor);
  yy_destructor(40,&yymsp[-3].minor);
  yy_destructor(26,&yymsp[-1].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3675 "parser.c"
        break;
      case 147:
// 1473 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_declare_statement(XX_T_TYPE_INTEGER, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(43,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3684 "parser.c"
        break;
      case 148:
// 1477 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_declare_statement(XX_T_TYPE_UINTEGER, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(81,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3693 "parser.c"
        break;
      case 149:
// 1481 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_declare_statement(XX_T_TYPE_CHAR, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(82,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3702 "parser.c"
        break;
      case 150:
// 1485 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_declare_statement(XX_T_TYPE_UCHAR, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(83,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3711 "parser.c"
        break;
      case 151:
// 1489 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_declare_statement(XX_T_TYPE_LONG, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(84,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3720 "parser.c"
        break;
      case 152:
// 1493 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_declare_statement(XX_T_TYPE_ULONG, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(85,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3729 "parser.c"
        break;
      case 153:
// 1497 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_declare_statement(XX_T_TYPE_DOUBLE, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(44,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3738 "parser.c"
        break;
      case 154:
// 1501 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_declare_statement(XX_T_TYPE_STRING, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(46,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3747 "parser.c"
        break;
      case 155:
// 1505 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_declare_statement(XX_T_TYPE_BOOL, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(45,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3756 "parser.c"
        break;
      case 156:
// 1509 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_declare_statement(XX_T_TYPE_VAR, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(48,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[0].minor);
}
// 3765 "parser.c"
        break;
      case 159:
// 1521 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_declare_variable(yymsp[0].minor.yy0, NULL, status->scanner_state);
}
// 3772 "parser.c"
        break;
      case 160:
// 1525 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_declare_variable(yymsp[-2].minor.yy0, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(36,&yymsp[-1].minor);
}
// 3780 "parser.c"
        break;
      case 162:
// 1533 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("not", yymsp[0].minor.yy115, NULL, NULL, status->scanner_state);
  yy_destructor(25,&yymsp[-1].minor);
}
// 3788 "parser.c"
        break;
      case 163:
// 1537 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("isset", yymsp[0].minor.yy115, NULL, NULL, status->scanner_state);
  yy_destructor(20,&yymsp[-1].minor);
}
// 3796 "parser.c"
        break;
      case 164:
// 1541 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("equals", yymsp[-2].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(9,&yymsp[-1].minor);
}
// 3804 "parser.c"
        break;
      case 165:
// 1545 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("not-equals", yymsp[-2].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(16,&yymsp[-1].minor);
}
// 3812 "parser.c"
        break;
      case 166:
// 1549 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("identical", yymsp[-2].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(10,&yymsp[-1].minor);
}
// 3820 "parser.c"
        break;
      case 167:
// 1553 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("not-identical", yymsp[-2].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(15,&yymsp[-1].minor);
}
// 3828 "parser.c"
        break;
      case 168:
// 1557 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("less", yymsp[-2].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(11,&yymsp[-1].minor);
}
// 3836 "parser.c"
        break;
      case 169:
// 1561 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("greater", yymsp[-2].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(12,&yymsp[-1].minor);
}
// 3844 "parser.c"
        break;
      case 170:
// 1565 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("less-equal", yymsp[-2].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(13,&yymsp[-1].minor);
}
// 3852 "parser.c"
        break;
      case 171:
// 1569 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("greater-equal", yymsp[-2].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(14,&yymsp[-1].minor);
}
// 3860 "parser.c"
        break;
      case 172:
// 1573 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("list", yymsp[-1].minor.yy115, NULL, NULL, status->scanner_state);
  yy_destructor(40,&yymsp[-2].minor);
  yy_destructor(26,&yymsp[0].minor);
}
// 3869 "parser.c"
        break;
      case 173:
// 1577 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("cast", yymsp[-2].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(40,&yymsp[-3].minor);
  yy_destructor(26,&yymsp[-1].minor);
}
// 3878 "parser.c"
        break;
      case 174:
// 1581 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("property-access", xx_ret_literal(XX_T_IDENTIFIER, yymsp[-2].minor.yy0, status->scanner_state), xx_ret_literal(XX_T_IDENTIFIER, yymsp[0].minor.yy0, status->scanner_state), NULL, status->scanner_state);
  yy_destructor(69,&yymsp[-1].minor);
}
// 3886 "parser.c"
        break;
      case 175:
// 1585 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("static-property-access", xx_ret_literal(XX_T_IDENTIFIER, yymsp[-2].minor.yy0, status->scanner_state), xx_ret_literal(XX_T_IDENTIFIER, yymsp[0].minor.yy0, status->scanner_state), NULL, status->scanner_state);
  yy_destructor(72,&yymsp[-1].minor);
}
// 3894 "parser.c"
        break;
      case 176:
// 1589 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("static-constant-access", xx_ret_literal(XX_T_IDENTIFIER, yymsp[-2].minor.yy0, status->scanner_state), xx_ret_literal(XX_T_IDENTIFIER, yymsp[0].minor.yy0, status->scanner_state), NULL, status->scanner_state);
  yy_destructor(72,&yymsp[-1].minor);
}
// 3902 "parser.c"
        break;
      case 177:
      case 228:
// 1593 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("array-access", xx_ret_literal(XX_T_IDENTIFIER, yymsp[-3].minor.yy0, status->scanner_state), xx_ret_literal(XX_T_IDENTIFIER, yymsp[-1].minor.yy0, status->scanner_state), NULL, status->scanner_state);
  yy_destructor(70,&yymsp[-2].minor);
  yy_destructor(71,&yymsp[0].minor);
}
// 3912 "parser.c"
        break;
      case 178:
      case 229:
// 1598 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("array-access", xx_ret_literal(XX_T_IDENTIFIER, yymsp[-3].minor.yy0, status->scanner_state), xx_ret_literal(XX_T_INTEGER, yymsp[-1].minor.yy0, status->scanner_state), NULL, status->scanner_state);
  yy_destructor(70,&yymsp[-2].minor);
  yy_destructor(71,&yymsp[0].minor);
}
// 3922 "parser.c"
        break;
      case 179:
      case 230:
// 1603 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("array-access", xx_ret_literal(XX_T_IDENTIFIER, yymsp[-3].minor.yy0, status->scanner_state), xx_ret_literal(XX_T_STRING, yymsp[-1].minor.yy0, status->scanner_state), NULL, status->scanner_state);
  yy_destructor(70,&yymsp[-2].minor);
  yy_destructor(71,&yymsp[0].minor);
}
// 3932 "parser.c"
        break;
      case 180:
// 1608 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("add", yymsp[-2].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(17,&yymsp[-1].minor);
}
// 3940 "parser.c"
        break;
      case 181:
// 1613 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("sub", yymsp[-2].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(18,&yymsp[-1].minor);
}
// 3948 "parser.c"
        break;
      case 182:
// 1618 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("concat", yymsp[-2].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(19,&yymsp[-1].minor);
}
// 3956 "parser.c"
        break;
      case 183:
// 1623 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("and", yymsp[-2].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(7,&yymsp[-1].minor);
}
// 3964 "parser.c"
        break;
      case 184:
// 1628 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("or", yymsp[-2].minor.yy115, yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(8,&yymsp[-1].minor);
}
// 3972 "parser.c"
        break;
      case 185:
// 1633 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("instanceof", yymsp[0].minor.yy115, NULL, NULL, status->scanner_state);
  yy_destructor(23,&yymsp[-1].minor);
}
// 3980 "parser.c"
        break;
      case 186:
// 1638 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("fetch", xx_ret_literal(XX_T_IDENTIFIER, yymsp[-2].minor.yy0, status->scanner_state), yymsp[0].minor.yy115, NULL, status->scanner_state);
  yy_destructor(21,&yymsp[-3].minor);
  yy_destructor(6,&yymsp[-1].minor);
}
// 3989 "parser.c"
        break;
      case 187:
// 1643 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("typeof", yymsp[0].minor.yy115, NULL, NULL, status->scanner_state);
  yy_destructor(22,&yymsp[-1].minor);
}
// 3997 "parser.c"
        break;
      case 188:
// 1648 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_literal(XX_T_CONSTANT, yymsp[0].minor.yy0, status->scanner_state);
}
// 4004 "parser.c"
        break;
      case 189:
      case 217:
      case 221:
// 1653 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_literal(XX_T_IDENTIFIER, yymsp[0].minor.yy0, status->scanner_state);
}
// 4013 "parser.c"
        break;
      case 190:
      case 219:
      case 222:
// 1658 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_literal(XX_T_INTEGER, yymsp[0].minor.yy0, status->scanner_state);
}
// 4022 "parser.c"
        break;
      case 191:
      case 218:
      case 223:
// 1663 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_literal(XX_T_STRING, yymsp[0].minor.yy0, status->scanner_state);
}
// 4031 "parser.c"
        break;
      case 192:
// 1668 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_literal(XX_T_SCHAR, yymsp[0].minor.yy0, status->scanner_state);
}
// 4038 "parser.c"
        break;
      case 193:
      case 224:
// 1673 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_literal(XX_T_DOUBLE, yymsp[0].minor.yy0, status->scanner_state);
}
// 4046 "parser.c"
        break;
      case 194:
      case 225:
// 1678 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_literal(XX_T_NULL, NULL, status->scanner_state);
  yy_destructor(88,&yymsp[0].minor);
}
// 4055 "parser.c"
        break;
      case 195:
      case 227:
// 1683 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_literal(XX_T_TRUE, NULL, status->scanner_state);
  yy_destructor(89,&yymsp[0].minor);
}
// 4064 "parser.c"
        break;
      case 196:
      case 226:
// 1688 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_literal(XX_T_FALSE, NULL, status->scanner_state);
  yy_destructor(90,&yymsp[0].minor);
}
// 4073 "parser.c"
        break;
      case 197:
// 1693 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("empty-array", NULL, NULL, NULL, status->scanner_state);
  yy_destructor(70,&yymsp[-1].minor);
  yy_destructor(71,&yymsp[0].minor);
}
// 4082 "parser.c"
        break;
      case 198:
// 1698 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_expr("array", yymsp[-1].minor.yy115, NULL, NULL, status->scanner_state);
  yy_destructor(70,&yymsp[-2].minor);
  yy_destructor(71,&yymsp[0].minor);
}
// 4091 "parser.c"
        break;
      case 199:
// 1703 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_new_instance(yymsp[0].minor.yy0, NULL, status->scanner_state);
  yy_destructor(24,&yymsp[-1].minor);
}
// 4099 "parser.c"
        break;
      case 200:
// 1708 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_new_instance(yymsp[-2].minor.yy0, NULL, status->scanner_state);
  yy_destructor(24,&yymsp[-3].minor);
  yy_destructor(40,&yymsp[-1].minor);
  yy_destructor(26,&yymsp[0].minor);
}
// 4109 "parser.c"
        break;
      case 201:
// 1713 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_new_instance(yymsp[-3].minor.yy0, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(24,&yymsp[-4].minor);
  yy_destructor(40,&yymsp[-2].minor);
  yy_destructor(26,&yymsp[0].minor);
}
// 4119 "parser.c"
        break;
      case 202:
// 1718 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_fcall(yymsp[-3].minor.yy0, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(40,&yymsp[-2].minor);
  yy_destructor(26,&yymsp[0].minor);
}
// 4128 "parser.c"
        break;
      case 203:
// 1723 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_fcall(yymsp[-2].minor.yy0, NULL, status->scanner_state);
  yy_destructor(40,&yymsp[-1].minor);
  yy_destructor(26,&yymsp[0].minor);
}
// 4137 "parser.c"
        break;
      case 206:
// 1738 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_scall(yymsp[-5].minor.yy0, yymsp[-3].minor.yy0, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(72,&yymsp[-4].minor);
  yy_destructor(40,&yymsp[-2].minor);
  yy_destructor(26,&yymsp[0].minor);
}
// 4147 "parser.c"
        break;
      case 207:
// 1743 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_scall(yymsp[-4].minor.yy0, yymsp[-2].minor.yy0, NULL, status->scanner_state);
  yy_destructor(72,&yymsp[-3].minor);
  yy_destructor(40,&yymsp[-1].minor);
  yy_destructor(26,&yymsp[0].minor);
}
// 4157 "parser.c"
        break;
      case 208:
// 1748 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_mcall(yymsp[-5].minor.yy0, yymsp[-3].minor.yy0, yymsp[-1].minor.yy115, status->scanner_state);
  yy_destructor(69,&yymsp[-4].minor);
  yy_destructor(40,&yymsp[-2].minor);
  yy_destructor(26,&yymsp[0].minor);
}
// 4167 "parser.c"
        break;
      case 209:
// 1753 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_mcall(yymsp[-4].minor.yy0, yymsp[-2].minor.yy0, NULL, status->scanner_state);
  yy_destructor(69,&yymsp[-3].minor);
  yy_destructor(40,&yymsp[-1].minor);
  yy_destructor(26,&yymsp[0].minor);
}
// 4177 "parser.c"
        break;
      case 215:
// 1777 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_array_item(yymsp[-2].minor.yy115, yymsp[0].minor.yy115, status->scanner_state);
  yy_destructor(55,&yymsp[-1].minor);
}
// 4185 "parser.c"
        break;
      case 216:
// 1781 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_array_item(NULL, yymsp[0].minor.yy115, status->scanner_state);
}
// 4192 "parser.c"
        break;
      case 232:
// 1848 "parser.lemon"
{
	yygotominor.yy115 = xx_ret_comment(yymsp[0].minor.yy0, status->scanner_state);
}
// 4199 "parser.c"
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yypParser,yygoto);
  if( yyact < YYNSTATE ){
    yy_shift(yypParser,yyact,yygoto,&yygotominor);
  }else if( yyact == YYNSTATE + YYNRULE + 1 ){
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  xx_ARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  xx_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  xx_ARG_FETCH;
#define TOKEN (yyminor.yy0)
// 801 "parser.lemon"


	//fprintf(stderr, "error!\n");

	json_object *syntax_error = json_object_new_object();

	json_object_object_add(syntax_error, "type", json_object_new_string("error"));

	/*if (status->scanner_state->start_length) {
		fprintf(stderr, "Syntax error, %s", status->scanner_state->start);
	} else {
		fprintf(stderr, "EOF");
	}*/

	//status->syntax_error_len = 48 + Z_STRLEN_P(status->scanner_state->active_file);
	//status->syntax_error = emalloc(sizeof(char) * status->syntax_error_len);

	if (status->scanner_state->start_length) {
		json_object_object_add(syntax_error, "message", json_object_new_string("Syntax error"));
	} else {
		json_object_object_add(syntax_error, "message", json_object_new_string("Unexpected EOF"));
	}

	json_object_object_add(syntax_error, "file", json_object_new_string(status->scanner_state->active_file));
	json_object_object_add(syntax_error, "line", json_object_new_int(status->scanner_state->active_line));
	json_object_object_add(syntax_error, "char", json_object_new_int(status->scanner_state->active_char));

	status->status = XX_PARSING_FAILED;

	status->ret = syntax_error;

	//status->scanner_state->active_file

// 4275 "parser.c"
  xx_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  xx_ARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  xx_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "xx_Alloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void xx_(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  xx_TOKENTYPE yyminor       /* The value for the token */
  xx_ARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
    if( yymajor==0 ) return;
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  xx_ARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,yymajor);
    if( yyact<YYNSTATE ){
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      if( yyendofinput && yypParser->yyidx>=0 ){
        yymajor = 0;
      }else{
        yymajor = YYNOCODE;
      }
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else if( yyact == YY_ERROR_ACTION ){
      int yymx;
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_shift_action(yypParser,YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }else{
      yy_accept(yypParser);
      yymajor = YYNOCODE;
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}

/*
 +----------------------------------------------------------------------+
 | Zephir Language                                                      |
 +----------------------------------------------------------------------+
 | Copyright (c) 2013 Zephir Team                                       |
 +----------------------------------------------------------------------+
 | This source file is subject to version 1.0 of the Zephir license,    |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.zephir-lang.com/license                                   |
 |                                                                      |
 | If you did not receive a copy of the Zephir license and are unable   |
 | to obtain it through the world-wide-web, please send a note to       |
 | license@zephir-lang.com so we can mail you a copy immediately.       |
 +----------------------------------------------------------------------+
*/

#define SUCCESS 1
#define FAILURE 0

const xx_token_names xx_tokens[] =
{
	{ XX_T_INTEGER,       		"INTEGER" },
	{ XX_T_DOUBLE,        		"DOUBLE" },
	{ XX_T_STRING,        		"STRING" },
	{ XX_T_IDENTIFIER,    		"IDENTIFIER" },
	{ XX_T_AT,	   	    		"@" },
	{ XX_T_COMMA,	   	    	"," },
	{ XX_T_ASSIGN,       		"=" },
	{ XX_T_COLON,            	":" },
	{ XX_T_PARENTHESES_OPEN,  	"(" },
	{ XX_T_PARENTHESES_CLOSE, 	")" },
	{ XX_T_BRACKET_OPEN,     	"{" },
	{ XX_T_BRACKET_CLOSE,    	"}" },
 	{ XX_T_SBRACKET_OPEN,    	"[" },
	{ XX_T_SBRACKET_CLOSE,   	"]" },
	{  0, NULL }
};

/**
 * Wrapper to alloc memory within the parser
 */
static void *xx_wrapper_alloc(size_t bytes){
	return malloc(bytes);
}

/**
 * Wrapper to free memory within the parser
 */
static void xx_wrapper_free(void *pointer){
	//free(pointer);
}

/**
 * Creates a parser_token to be passed to the parser
 */
static void xx_parse_with_token(void* xx_parser, int opcode, int parsercode, xx_scanner_token *token, xx_parser_status *parser_status){

	xx_parser_token *pToken;

	pToken = malloc(sizeof(xx_parser_token));
	pToken->opcode = opcode;
	pToken->token = token->value;
	pToken->token_len = token->len;
	pToken->free_flag = 1;

	xx_(xx_parser, parsercode, pToken, parser_status);

	token->value = NULL;
	token->len = 0;
}

/**
 * Creates an error message when it's triggered by the scanner
 */
static void xx_scanner_error_msg(xx_parser_status *parser_status){

	/*char *error, *error_part;
	XX_scanner_state *state = parser_status->scanner_state;

	//PHALCON_INIT_VAR(*error_msg);
	ALLOC_INIT_ZVAL(*error_msg);
	if (state->start) {
		error = emalloc(sizeof(char) * (128 + state->start_length +  Z_STRLEN_P(state->active_file)));
		if (state->start_length > 16) {
			error_part = estrndup(state->start, 16);
			sprintf(error, "Parsing error before '%s...' in %s on line %d", error_part, Z_STRVAL_P(state->active_file), state->active_line);
			efree(error_part);
		} else {
			sprintf(error, "Parsing error before '%s' in %s on line %d", state->start, Z_STRVAL_P(state->active_file), state->active_line);
		}
		ZVAL_STRING(*error_msg, error, 1);
	} else {
		error = emalloc(sizeof(char) * (64 + Z_STRLEN_P(state->active_file)));
		sprintf(error, "Parsing error near to EOF in %s", Z_STRVAL_P(state->active_file));
		ZVAL_STRING(*error_msg, error, 1);
	}
	efree(error);*/
}

/**
 * Parses a comment returning an intermediate array representation
 */
int xx_parse_program(char *program, unsigned int program_length, char *file_path) {

	char *error;
	xx_scanner_state *state;
	xx_scanner_token token;
	int scanner_status, status = SUCCESS, start_lines;
	xx_parser_status *parser_status = NULL;
	void* xx_parser;

	/**
	 * Check if the program has any length
	 */
	if (!program_length) {
		return FAILURE;
	}

	if (program_length < 2) {
		return SUCCESS;
	}

	/**
	 * Start the reentrant parser
	 */
	xx_parser = xx_Alloc(xx_wrapper_alloc);

	parser_status = malloc(sizeof(xx_parser_status));
	state = malloc(sizeof(xx_scanner_state));

	parser_status->status = XX_PARSING_OK;
	parser_status->scanner_state = state;
	parser_status->ret = NULL;
	parser_status->token = &token;
	parser_status->syntax_error = NULL;
	parser_status->number_brackets = 0;

	/**
	 * Initialize the scanner state
	 */
	state->active_token = 0;
	state->start = program;
	state->start_length = 0;
	state->active_file = file_path;
	state->active_line = 1;
	state->active_char = 1;

	state->end = state->start;

	while (0 <= (scanner_status = xx_get_token(state, &token))) {

		state->active_token = token.opcode;

		state->start_length = (program + program_length - state->start);

		switch (token.opcode) {

			case XX_T_IGNORE:
				break;

			case XX_T_NAMESPACE:
				xx_(xx_parser, XX_NAMESPACE, NULL, parser_status);
				break;
			case XX_T_ABSTRACT:
				xx_(xx_parser, XX_ABSTRACT, NULL, parser_status);
				break;
			case XX_T_CLASS:
				xx_(xx_parser, XX_CLASS, NULL, parser_status);
				break;
			case XX_T_EXTENDS:
				xx_(xx_parser, XX_EXTENDS, NULL, parser_status);
				break;
			case XX_T_PUBLIC:
				xx_(xx_parser, XX_PUBLIC, NULL, parser_status);
				break;
			case XX_T_PROTECTED:
				xx_(xx_parser, XX_PROTECTED, NULL, parser_status);
				break;
			case XX_T_PRIVATE:
				xx_(xx_parser, XX_PRIVATE, NULL, parser_status);
				break;
			case XX_T_STATIC:
				xx_(xx_parser, XX_STATIC, NULL, parser_status);
				break;
			case XX_T_INLINE:
				xx_(xx_parser, XX_INLINE, NULL, parser_status);
				break;
			case XX_T_FINAL:
				xx_(xx_parser, XX_FINAL, NULL, parser_status);
				break;
			case XX_T_FUNCTION:
				xx_(xx_parser, XX_FUNCTION, NULL, parser_status);
				break;
			case XX_T_LET:
				xx_(xx_parser, XX_LET, NULL, parser_status);
				break;
			case XX_T_ECHO:
				xx_(xx_parser, XX_ECHO, NULL, parser_status);
				break;
			case XX_T_RETURN:
				xx_(xx_parser, XX_RETURN, NULL, parser_status);
				break;
			case XX_T_IF:
				xx_(xx_parser, XX_IF, NULL, parser_status);
				break;
			case XX_T_ELSE:
				xx_(xx_parser, XX_ELSE, NULL, parser_status);
				break;
			case XX_T_LOOP:
				xx_(xx_parser, XX_LOOP, NULL, parser_status);
				break;
			case XX_T_CONTINUE:
				xx_(xx_parser, XX_CONTINUE, NULL, parser_status);
				break;
			case XX_T_BREAK:
				xx_(xx_parser, XX_BREAK, NULL, parser_status);
				break;
			case XX_T_WHILE:
				xx_(xx_parser, XX_WHILE, NULL, parser_status);
				break;
			case XX_T_DO:
				xx_(xx_parser, XX_DO, NULL, parser_status);
				break;
			case XX_T_NEW:
				xx_(xx_parser, XX_NEW, NULL, parser_status);
				break;
			case XX_T_CONST:
				xx_(xx_parser, XX_CONST, NULL, parser_status);
				break;
			case XX_T_TYPEOF:
				xx_(xx_parser, XX_TYPEOF, NULL, parser_status);
				break;
			case XX_T_INSTANCEOF:
				xx_(xx_parser, XX_INSTANCEOF, NULL, parser_status);
				break;
			case XX_T_ISSET:
				xx_(xx_parser, XX_ISSET, NULL, parser_status);
				break;
			case XX_T_UNSET:
				xx_(xx_parser, XX_UNSET, NULL, parser_status);
				break;
			case XX_T_THROW:
				xx_(xx_parser, XX_THROW, NULL, parser_status);
				break;
			case XX_T_FOR:
				xx_(xx_parser, XX_FOR, NULL, parser_status);
				break;
			case XX_T_IN:
				xx_(xx_parser, XX_IN, NULL, parser_status);
				break;
			case XX_T_REVERSE:
				xx_(xx_parser, XX_REVERSE, NULL, parser_status);
				break;

			case XX_T_DOTCOMMA:
				xx_(xx_parser, XX_DOTCOMMA, NULL, parser_status);
				break;
			case XX_T_COMMA:
				xx_(xx_parser, XX_COMMA, NULL, parser_status);
				break;
			case XX_T_ASSIGN:
				xx_(xx_parser, XX_ASSIGN, NULL, parser_status);
				break;
			case XX_T_ADDASSIGN:
				xx_(xx_parser, XX_ADDASSIGN, NULL, parser_status);
				break;
			case XX_T_SUBASSIGN:
				xx_(xx_parser, XX_SUBASSIGN, NULL, parser_status);
				break;
			case XX_T_MULASSIGN:
				xx_(xx_parser, XX_MULASSIGN, NULL, parser_status);
				break;
			case XX_T_CONCATASSIGN:
				xx_(xx_parser, XX_CONCATASSIGN, NULL, parser_status);
				break;
			case XX_T_EQUALS:
				xx_(xx_parser, XX_EQUALS, NULL, parser_status);
				break;
			case XX_T_NOTEQUALS:
				xx_(xx_parser, XX_NOTEQUALS, NULL, parser_status);
				break;
			case XX_T_IDENTICAL:
				xx_(xx_parser, XX_IDENTICAL, NULL, parser_status);
				break;
			case XX_T_NOTIDENTICAL:
				xx_(xx_parser, XX_NOTIDENTICAL, NULL, parser_status);
				break;
			case XX_T_LESS:
				xx_(xx_parser, XX_LESS, NULL, parser_status);
				break;
			case XX_T_GREATER:
				xx_(xx_parser, XX_GREATER, NULL, parser_status);
				break;
			case XX_T_LESSEQUAL:
				xx_(xx_parser, XX_LESSEQUAL, NULL, parser_status);
				break;
			case XX_T_GREATEREQUAL:
				xx_(xx_parser, XX_GREATEREQUAL, NULL, parser_status);
				break;
			case XX_T_COLON:
				xx_(xx_parser, XX_COLON, NULL, parser_status);
				break;
			case XX_T_ARROW:
				xx_(xx_parser, XX_ARROW, NULL, parser_status);
				break;
			case XX_T_DOUBLECOLON:
				xx_(xx_parser, XX_DOUBLECOLON, NULL, parser_status);
				break;
			case XX_T_NOT:
				xx_(xx_parser, XX_NOT, NULL, parser_status);
				break;
			case XX_T_FETCH:
				xx_(xx_parser, XX_FETCH, NULL, parser_status);
				break;
			case XX_T_SWITCH:
				xx_(xx_parser, XX_SWITCH, NULL, parser_status);
				break;
			case XX_T_CASE:
				xx_(xx_parser, XX_CASE, NULL, parser_status);
				break;
			case XX_T_DEFAULT:
				xx_(xx_parser, XX_DEFAULT, NULL, parser_status);
				break;

			case XX_T_PARENTHESES_OPEN:
				xx_(xx_parser, XX_PARENTHESES_OPEN, NULL, parser_status);
				break;
			case XX_T_PARENTHESES_CLOSE:
				xx_(xx_parser, XX_PARENTHESES_CLOSE, NULL, parser_status);
				break;

			case XX_T_BRACKET_OPEN:
				parser_status->number_brackets++;
				xx_(xx_parser, XX_BRACKET_OPEN, NULL, parser_status);
				break;
			case XX_T_BRACKET_CLOSE:
				parser_status->number_brackets--;
				xx_(xx_parser, XX_BRACKET_CLOSE, NULL, parser_status);
				break;

			case XX_T_SBRACKET_OPEN:
				xx_(xx_parser, XX_SBRACKET_OPEN, NULL, parser_status);
				break;
			case XX_T_SBRACKET_CLOSE:
				xx_(xx_parser, XX_SBRACKET_CLOSE, NULL, parser_status);
				break;

			case XX_T_NULL:
				xx_(xx_parser, XX_NULL, NULL, parser_status);
				break;
			case XX_T_TRUE:
				xx_(xx_parser, XX_TRUE, NULL, parser_status);
				break;
			case XX_T_FALSE:
				xx_(xx_parser, XX_FALSE, NULL, parser_status);
				break;
			case XX_T_COMMENT:
				if (parser_status->number_brackets <= 1) {
					xx_parse_with_token(xx_parser, XX_T_COMMENT, XX_COMMENT, &token, parser_status);
				}
				break;

			case XX_T_TYPE_INTEGER:
				xx_(xx_parser, XX_TYPE_INTEGER, NULL, parser_status);
				break;
			case XX_T_TYPE_UINTEGER:
				xx_(xx_parser, XX_TYPE_UINTEGER, NULL, parser_status);
				break;
			case XX_T_TYPE_CHAR:
				xx_(xx_parser, XX_TYPE_CHAR, NULL, parser_status);
				break;
			case XX_T_TYPE_UCHAR:
				xx_(xx_parser, XX_TYPE_UCHAR, NULL, parser_status);
				break;
			case XX_T_TYPE_LONG:
				xx_(xx_parser, XX_TYPE_LONG, NULL, parser_status);
				break;
			case XX_T_TYPE_ULONG:
				xx_(xx_parser, XX_TYPE_ULONG, NULL, parser_status);
				break;
			case XX_T_TYPE_DOUBLE:
				xx_(xx_parser, XX_TYPE_DOUBLE, NULL, parser_status);
				break;
			case XX_T_TYPE_STRING:
				xx_(xx_parser, XX_TYPE_STRING, NULL, parser_status);
				break;
			case XX_T_TYPE_BOOL:
				xx_(xx_parser, XX_TYPE_BOOL, NULL, parser_status);
				break;
			case XX_T_TYPE_VAR:
				xx_(xx_parser, XX_TYPE_VAR, NULL, parser_status);
				break;

			case XX_T_ADD:
				xx_(xx_parser, XX_ADD, NULL, parser_status);
				break;
			case XX_T_SUB:
				xx_(xx_parser, XX_SUB, NULL, parser_status);
				break;
			case XX_T_DOT:
				xx_(xx_parser, XX_CONCAT, NULL, parser_status);
				break;
			case XX_T_INCR:
				xx_(xx_parser, XX_INCR, NULL, parser_status);
				break;
			case XX_T_DECR:
				xx_(xx_parser, XX_DECR, NULL, parser_status);
				break;
			case XX_T_AND:
				xx_(xx_parser, XX_AND, NULL, parser_status);
				break;
			case XX_T_OR:
				xx_(xx_parser, XX_OR, NULL, parser_status);
				break;

			case XX_T_INTEGER:
				xx_parse_with_token(xx_parser, XX_T_INTEGER, XX_INTEGER, &token, parser_status);
				break;
			case XX_T_DOUBLE:
				xx_parse_with_token(xx_parser, XX_T_DOUBLE, XX_DOUBLE, &token, parser_status);
				break;
			case XX_T_STRING:
				xx_parse_with_token(xx_parser, XX_T_STRING, XX_STRING, &token, parser_status);
				break;
			case XX_T_SCHAR:
				xx_parse_with_token(xx_parser, XX_T_SCHAR, XX_SCHAR, &token, parser_status);
				break;
			case XX_T_IDENTIFIER:
				xx_parse_with_token(xx_parser, XX_T_IDENTIFIER, XX_IDENTIFIER, &token, parser_status);
				break;
			case XX_T_CONSTANT:
				xx_parse_with_token(xx_parser, XX_T_CONSTANT, XX_CONSTANT, &token, parser_status);
				break;

			default:
				parser_status->status = XX_PARSING_FAILED;
				fprintf(stderr, "Scanner: unknown opcode %d\n", token.opcode);
				/*if (!*error_msg) {
					error = emalloc(sizeof(char) * (48 + Z_STRLEN_P(state->active_file)));
					sprintf(error, "Scanner: unknown opcode %d on in %s line %d", token.opcode, Z_STRVAL_P(state->active_file), state->active_line);
					//PHALCON_INIT_VAR(*error_msg);
					ALLOC_INIT_ZVAL(*error_msg);
					ZVAL_STRING(*error_msg, error, 1);
					efree(error);
				}*/
				break;
		}

		if (parser_status->status != XX_PARSING_OK) {
			status = FAILURE;
			break;
		}

		state->end = state->start;
	}

	if (status != FAILURE) {
		switch (scanner_status) {
			case XX_SCANNER_RETCODE_ERR:
			case XX_SCANNER_RETCODE_IMPOSSIBLE:
				{
					char *x = malloc(sizeof(char) * 10000);
					if (state->start) {
						sprintf(x, "Scanner error: %d %s", scanner_status, state->start);
					} else {
						sprintf(x, "Scanner error: %d", scanner_status);
					}
					json_object *syntax_error = json_object_new_object();
					json_object_object_add(syntax_error, "type", json_object_new_string("error"));
					json_object_object_add(syntax_error, "message", json_object_new_string(x));
					//free(x);
					parser_status->ret = syntax_error;
					status = FAILURE;
				}
				break;
			default:
				xx_(xx_parser, 0, NULL, parser_status);
		}
	}

	state->active_token = 0;
	state->start = NULL;

	if (parser_status->status != XX_PARSING_OK) {
		status = FAILURE;
		/*if (parser_status->syntax_error) {
			if (!*error_msg) {
				//PHALCON_INIT_VAR(*error_msg);
				ALLOC_INIT_ZVAL(*error_msg);
				ZVAL_STRING(*error_msg, parser_status->syntax_error, 1);
			}
			efree(parser_status->syntax_error);
		}*/
		//fprintf(stderr, "error!\n");
	}

	xx_Free(xx_parser, xx_wrapper_free);

	if (status != FAILURE) {
		if (parser_status->status == XX_PARSING_OK) {
			//printf("%s\n", json_object_to_json_string(parser_status->ret));
			/*if (parser_status->ret) {
				ZVAL_ZVAL(*result, parser_status->ret, 0, 0);
				ZVAL_NULL(parser_status->ret);
				zval_ptr_dtor(&parser_status->ret);
			} else {
				array_init(*result);
			}*/
		}
	}

	if (parser_status->ret) {
		printf("%s\n", json_object_to_json_string(parser_status->ret));
	}

	//efree(Z_STRVAL(processed_comment));*/

	free(parser_status);
	free(state);

	return status;
}

int main(int argc, char **argv) {

	FILE *fp;
	char ch;
	char *program;
	int i, length;

	if (argc > 0) {

		fp = fopen(argv[1], "r");
		if (!fp) {
			fprintf(stderr, "Cant open file\n");
			exit(1);
		}

		length = 1024;
		program = malloc(sizeof(char) * length);

		i = 0;
		while (!feof(fp)) {
			ch = fgetc(fp);
			if (i == length) {
				length += 1024;
				program = realloc(program, sizeof(char) * length);
			}
			program[i++] = ch;
		}
		program[i - 1] = '\0';
		fclose(fp);

		xx_parse_program(program, i - 1, argv[1]);

		free(program);
	}

}