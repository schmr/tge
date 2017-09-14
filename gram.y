%{
#include <stdio.h>
#include <cstring>
#include "ArgPack.h"
#define YYDEBUG 1
#include "lex_yacc.h"

int yylex();
void yyerror(char *s);
int in_braces = 0;
int in_struct = 0;
%}

%token IDENTIFIER 
%token CONSTANT STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN

%token TYPE_NAME
%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%start file
%%

file
	: external_definition { yypopulate_tree("file", 1); }
	| file external_definition { yypopulate_tree("file", 2); }
	;

external_definition
	: function_definition { yypopulate_tree("external_definition", 1); }
	| declaration { yypopulate_tree("external_definition", 1); }
	;

function_definition
	: declarator function_body { yypopulate_tree("function_definition", 2); }
	| declaration_specifiers declarator function_body {
			yypopulate_tree("function_definition", 3);
		}
	;

function_body
	: compound_statement { yypopulate_tree("function_body", 1); }
	| declaration_list compound_statement { yypopulate_tree("function_body", 2); }
	;

argument_expr_list
	: assignment_expr { yypopulate_tree("argument_expr_list", 1); }
	| argument_expr_list comma assignment_expr { yypopulate_tree("argument_expr_list", 3); }
	;

expr
	: assignment_expr { yypopulate_tree("expr", 1); }
	| expr comma assignment_expr { yypopulate_tree("expr", 3); }
	;

declaration
	: declaration_specifiers semi { yypopulate_tree("declaration", 2); }
	| declaration_specifiers init_declarator_list semi { yypopulate_tree("declaration", 3); }
	;

declaration_specifiers
	: storage_class_specifier { yypopulate_tree("declaration_specifiers", 1); }
	| storage_class_specifier declaration_specifiers { yypopulate_tree("declaration_specifiers", 2); }
	| type_specifier_list { yypopulate_tree("declaration_specifiers", 1); }
	;

init_declarator_list
	: init_declarator { yypopulate_tree("init_declarator_list", 1); }
	| init_declarator_list comma init_declarator { yypopulate_tree("init_declarator_list", 3); }
	;

init_declarator
	: declarator { yypopulate_tree("init_declarator", 1); }
	| declarator equal initializer { yypopulate_tree("init_declarator", 3); }
	;

storage_class_specifier
	: TYPEDEF 		{ yytypedef_flag = 1;  yypopulate_tree("typedef", 0); }
	| EXTERN 		{ yypopulate_tree("extern", 0); }
	| STATIC 		{ yypopulate_tree("static", 0); }
	;

type_specifier
	: CHAR   		{ yypopulate_tree("char", 0); }
	| SHORT  		{ yypopulate_tree("short", 0); }
	| INT  		{ yypopulate_tree("int", 0); }
	| LONG 		{ yypopulate_tree("long", 0); }
	| REGISTER 		{ yypopulate_tree("register", 0); }
	| SIGNED 		{ yypopulate_tree("signed", 0); }
	| UNSIGNED		{ yypopulate_tree("unsigned", 0); }
	| FLOAT  		{ yypopulate_tree("float", 0); }
	| DOUBLE 		{ yypopulate_tree("double", 0); }
	| CONST  		{ yypopulate_tree("const", 0); }
	| VOLATILE		{ yypopulate_tree("volatile", 0); }
	| VOID  		{ yypopulate_tree("void", 0); }
	| struct_or_union_specifier  { yypopulate_tree("struct_or_union", 1); }
	| enum_specifier  { yypopulate_tree("long", 1); }
	| TYPE_NAME		{ yypopulate_tree(*($1), 0); }
	;

struct_or_union_specifier
	: struct_or_union identifier lbrace struct_declaration_list rbrace   { yypopulate_tree("struct_or_union_specifier", 5); }
	| struct_or_union lbrace struct_declaration_list rbrace  { yypopulate_tree("struct_or_union_specifier", 4); }
	| struct_or_union identifier  { yypopulate_tree("struct_or_union_specifier", 2); }
	;

struct_or_union
	: STRUCT	{ in_struct++;  yypopulate_tree("struct", 0); }
	| UNION 		{ in_struct++; yypopulate_tree("union", 0); }
	;

struct_declaration_list
	: struct_declaration   { yypopulate_tree("struct_declaration_list", 1); }
	| struct_declaration_list struct_declaration   { yypopulate_tree("struct_declaration_list", 2); }
	;

struct_declaration
	: type_specifier_list struct_declarator_list semi   { yypopulate_tree("struct_declaration", 3); }
	;

struct_declarator_list
	: struct_declarator	 { yypopulate_tree("struct_declarator_list", 1); }
	| struct_declarator_list comma struct_declarator { yypopulate_tree("struct_declarator_list", 3); }
	;

struct_declarator
	: declarator	 { yypopulate_tree("struct_declarator", 1); }
	| colon constant_expr	 { yypopulate_tree("struct_declarator", 2); }
	| declarator colon constant_expr	 { yypopulate_tree("struct_declarator", 3); }
	;

enum_specifier
	: enum lbrace enumerator_list rbrace 	 { yypopulate_tree("enum_specifier", 4); }
	| enum identifier lbrace enumerator_list rbrace  { yypopulate_tree("enum_specifier", 5); }
	| enum identifier   { yypopulate_tree("enum_specifier", 1); }
	;

enumerator_list
	: enumerator   { yypopulate_tree("enumerator_list", 1); }
	| enumerator_list comma enumerator  { yypopulate_tree("enumerator_list", 3); }
	;

enumerator
	: identifier  { yypopulate_tree("enumerator", 1); }
	| identifier equal constant_expr   { yypopulate_tree("enumerator", 3); } 
	;

declarator
	: declarator2 { yypopulate_tree("declarator", 1); }
	| pointer declarator2 { yypopulate_tree("declarator", 2); }
	;

declarator2
	: identifier 	{ yypopulate_tree("identifier", 1); }
	| lparen declarator rparen	{ yypopulate_tree("declarator2", 3); }
	| declarator2 lbracket rbracket	{ yypopulate_tree("ARRAY", 3); }
	| declarator2 lbracket constant_expr rbracket		{ yypopulate_tree("array_decl", 4); }
	| declarator2 lparen rparen		{ yypopulate_tree("function", 3); }
	| declarator2 lparen parameter_list rparen		{ yypopulate_tree("function", 4); }
//	| declarator2 lparen parameter_type_list rparen		{ yypopulate_tree("function", 4); }
//	| declarator2 lparen parameter_identifier_list rparen 	{ yypopulate_tree("function", 4); }
	;

pointer
	: star		{ yypopulate_tree("pointer", 1); }
	| star type_specifier_list		{ yypopulate_tree("pointer", 2); } 
	| star pointer		{ yypopulate_tree("pointer", 2); } 
	| star type_specifier_list pointer		{ yypopulate_tree("pointer", 3); }
	;

type_specifier_list
	: type_specifier 		{ yypopulate_tree("type", 1); }
	| type_specifier_list type_specifier 		{ yypopulate_tree("type_specifier_list", 2); }
	;

//parameter_identifier_list
//	: identifier_list		{ yypopulate_tree("parameter_identifier_list", 1); }
//	| identifier_list comma elipsis		{ yypopulate_tree("parameter_identifier_list", 3); }
//	;

//identifier_list
//	: identifier		{ yypopulate_tree("parameter_declaration", 1); }
//	| identifier_list comma identifier 		{ yypopulate_tree("parameter_declaration", 3); }
//	;

//parameter_type_list
//	: parameter_list	{ yypopulate_tree("parameter_type_list", 1); }
//	| parameter_list comma elipsis	{ yypopulate_tree("parameter_type_list", 3); }
//	;

//parameter_list
//	: parameter_declaration	{ yypopulate_tree("parameter_list", 1); }
//	| parameter_list comma parameter_declaration 	{ yypopulate_tree("parameter_list", 3); }
//	;

parameter_list
	: parameter  { yypopulate_tree("parameter_list", 1); }
	| parameter_list comma parameter  { yypopulate_tree("parameter_list", 3); }
	;

parameter
	: parameter_declaration { yypopulate_tree("parameter", 1); }
	| identifier { yypopulate_tree("identifier", 1); }
	| elipsis { yypopulate_tree("parameter", 1); }
	;

parameter_declaration
	: type_specifier_list declarator 	{ yypopulate_tree("parameter_declaration", 2); }
	| type_name 	{ yypopulate_tree("parameter_declaration", 1); }
	;

type_name
	: type_specifier_list { yypopulate_tree("type_name", 1); }
	| type_specifier_list abstract_declarator { yypopulate_tree("type_name", 2); }
	;

abstract_declarator
	: pointer	{ yypopulate_tree("parameter_declarator", 1); }
	| abstract_declarator2	{ yypopulate_tree("parameter_declarator", 1); }
	| pointer abstract_declarator2	{ yypopulate_tree("parameter_declarator", 2); }
	;

abstract_declarator2
	: lparen abstract_declarator rparen		{ yypopulate_tree("abstract_declarator2", 3); }
	| lbracket rbracket			{ yypopulate_tree("abstract_declarator2", 2); }
	| lbracket constant_expr rbracket			{ yypopulate_tree("abstract_declarator2", 3); }
	| abstract_declarator2 lbracket rbracket			{ yypopulate_tree("abstract_declarator2", 3); }
	| abstract_declarator2 lbracket constant_expr rbracket			{ yypopulate_tree("abstract_declarator2", 4); }
	| lparen rparen			{ yypopulate_tree("abstract_declarator2", 2); }
///	| lparen parameter_list rparen			{ yypopulate_tree("abstract_declarator2", 3); }
//	| lparen parameter_type_list rparen			{ yypopulate_tree("abstract_declarator2", 3); }
	| abstract_declarator2 lparen rparen			{ yypopulate_tree("abstract_declarator2", 3); }
///	| abstract_declarator2 lparen parameter_list rparen			{ yypopulate_tree("abstract_declarator2", 4); }
//	| abstract_declarator2 lparen parameter_type_list rparen			{ yypopulate_tree("abstract_declarator2", 4); }
	;

initializer
	: assignment_expr		{ yypopulate_tree("initializer",1); }
	| lbrace initializer_list rbrace		{ yypopulate_tree("initializer",3); }
	| lbrace initializer_list comma rbrace		{ yypopulate_tree("initializer",4); }
	;

initializer_list
	: initializer		{ yypopulate_tree("initializer_list",1); }
	| initializer_list comma initializer		{ yypopulate_tree("initializer_list",3); }
	;

statement
	: labeled_statement		{ yypopulate_tree("statement",1); }
	| compound_statement		{ yypopulate_tree("statement",1); }
	| expression_statement		{ yypopulate_tree("statement",1); }
	| selection_statement		{ yypopulate_tree("statement",1); }
	| iteration_statement		{ yypopulate_tree("statement",1); }
	| jump_statement		{ yypopulate_tree("statement",1); }
	;

labeled_statement
	: identifier colon statement		{ yypopulate_tree("labeled_statement",3); }
	| case constant_expr colon statement		{ yypopulate_tree("labeled_statement",4); }
	| default colon statement		{ yypopulate_tree("labeled_statement",3); }
	;

compound_statement
	: lbrace rbrace		{ yypopulate_tree("compound_statement",2); }
	| lbrace statement_list rbrace		{ yypopulate_tree("compound_statement",3); }
	| lbrace declaration_list rbrace		{ yypopulate_tree("compound_statement",3); }
	| lbrace declaration_list statement_list rbrace		{ yypopulate_tree("compound_statement",4); }
	;

declaration_list
	: declaration		{ yypopulate_tree("declaration_list",1); }
	| declaration_list declaration		{ yypopulate_tree("declaration_list",2); }
	;

statement_list
	: statement		{ yypopulate_tree("statement_list",1); }
	| statement_list statement		{ yypopulate_tree("statement_list",2); }
	;

expression_statement
	: semi		{ yypopulate_tree("expression_statement",1); }
	| expr semi		{ yypopulate_tree("expression_statement",2); }
	;

selection_statement
	: if lparen expr rparen statement		{ yypopulate_tree("selection_statement",5); }
	| if lparen expr rparen statement else statement		{ yypopulate_tree("selection_statement",7); }
	| switch lparen expr rparen statement		{ yypopulate_tree("selection_statement",5); }
	;

iteration_statement
	: while lparen expr rparen statement		{ yypopulate_tree("iteration_statement",5); }
	| do statement while lparen expr rparen semi		{ yypopulate_tree("iteration_statement",7); }
	| for lparen semi semi rparen statement		{ yypopulate_tree("iteration_statement",6); }
	| for lparen semi semi expr rparen statement		{ yypopulate_tree("iteration_statement",7); }
	| for lparen semi expr semi rparen statement		{ yypopulate_tree("iteration_statement",7); }
	| for lparen semi expr semi expr rparen statement		{ yypopulate_tree("iteration_statement",8); }
	| for lparen expr semi semi rparen statement		{ yypopulate_tree("iteration_statement",7); }
	| for lparen expr semi semi expr rparen statement		{ yypopulate_tree("iteration_statement",8); }
	| for lparen expr semi expr semi rparen statement		{ yypopulate_tree("iteration_statement",8); }
	| for lparen expr semi expr semi expr rparen statement		{ yypopulate_tree("iteration_statement",9); }
	;

jump_statement
	: goto identifier semi		{ yypopulate_tree("jump_statement",3); }
	| continue semi		{ yypopulate_tree("jump_statement",2); }
	| break semi		{ yypopulate_tree("jump_statement",2); }
	| return semi		{ yypopulate_tree("jump_statement",2); }
	| return expr semi		{ yypopulate_tree("jump_statement",3); }
	;

identifier
	: IDENTIFIER	{
			yypopulate_tree(*($1), 0);
			if (in_struct) {
				in_struct = 0;
			} else if (yytypedef_flag && !in_braces) {
				yytypedef_table_add(*($1)); 
				yytypedef_flag = 0; 
			} else {
//				yypopulate_db (NEW_VAR, *($1));
			}
		}
	;

string
	: string STRING_LITERAL { yypopulate_tree("part_of_string", 1); }
	| STRING_LITERAL { yypopulate_tree(*($1), 0); }
	;

primary_expr
	: identifier { yypopulate_tree("primary_expr", 1); }
	| CONSTANT			{ yypopulate_tree(*($1), 0); }
	| string		{ yypopulate_tree("string", 1); }
	| lparen expr rparen	  { yypopulate_tree("primary_expr", 3); }
	;

postfix_expr
	: primary_expr  //KSV{ yypopulate_tree("postfix_expr", 1); }
	| postfix_expr lbracket expr rbracket	 { yypopulate_tree("postfix_expr", 4); }
	| postfix_expr lparen rparen 	 { yypopulate_tree("function_call", 3); }
	| postfix_expr lparen argument_expr_list rparen 	 { yypopulate_tree("function_call", 4); }
	| postfix_expr period identifier	 { yypopulate_tree("postfix_expr", 3); }
	| postfix_expr ptr_op identifier	 { yypopulate_tree("postfix_expr", 3); }
	| postfix_expr inc_op	 { yypopulate_tree("short_assign_op", 2); }
	| postfix_expr dec_op	 { yypopulate_tree("short_assign_op", 2); }
	;

unary_expr
	: postfix_expr	 //KSV{ yypopulate_tree("unary_expr", 1); }
	| inc_op unary_expr	 { yypopulate_tree("short_assign_op", 2); }
	| dec_op unary_expr	 { yypopulate_tree("short_assign_op", 2); }
	| unary_operator cast_expr	 { yypopulate_tree("unary_expr", 2); }
	| sizeof unary_expr	 { yypopulate_tree("unary_expr", 2); }
	| sizeof lparen type_name rparen	 { yypopulate_tree("unary_expr", 4); }
	;

unary_operator
	: amperstand	 { yypopulate_tree("unary_operator",1 ); }
	| star	 { yypopulate_tree("unary_operator",1 ); }
	| plus	 { yypopulate_tree("unary_operator",1 ); }
	| minus	 { yypopulate_tree("unary_operator",1 ); }
	| tilda	 { yypopulate_tree("unary_operator",1 ); }
	| emark	 { yypopulate_tree("unary_operator",1 ); }
	;

cast_expr
	: unary_expr	 //KSV{ yypopulate_tree("cast_expr", 1); }
	| lparen type_name rparen cast_expr	 { yypopulate_tree("cast_expr", 4); }
	;

multiplicative_expr
	: cast_expr	 //KSV{ yypopulate_tree("multiplicative_expr", 1); }
	| multiplicative_expr star cast_expr	 { yypopulate_tree("multiplicative_expr", 3); }
	| multiplicative_expr slash cast_expr	 { yypopulate_tree("multiplicative_expr", 3); }
	| multiplicative_expr percent cast_expr	 { yypopulate_tree("multiplicative_expr", 3); }
	;

additive_expr
	: multiplicative_expr	 //KSV{ yypopulate_tree("additive_expr", 1); }
	| additive_expr plus multiplicative_expr	 { yypopulate_tree("additive_expr", 3); }
	| additive_expr minus multiplicative_expr	 { yypopulate_tree("additive_expr", 3); }
	;

shift_expr
	: additive_expr	 //KSV{ yypopulate_tree("shift_expr", 1); }
	| shift_expr left_op additive_expr	 { yypopulate_tree("shift_expr", 3); }
	| shift_expr right_op additive_expr	 { yypopulate_tree("shift_expr", 3); }
	;

relational_expr
	: shift_expr	 //KSV{ yypopulate_tree("relational_expr", 1); }
	| relational_expr langle shift_expr	 { yypopulate_tree("relational_expr", 3); }
	| relational_expr rangle shift_expr	 { yypopulate_tree("relational_expr", 3); }
	| relational_expr le_op shift_expr	 { yypopulate_tree("relational_expr", 3); }
	| relational_expr ge_op shift_expr	 { yypopulate_tree("relational_expr", 3); }
	;

equality_expr
	: relational_expr	 //KSV{ yypopulate_tree("equality_expr", 1); }
	| equality_expr eq_op relational_expr	 { yypopulate_tree("equality_expr", 3); }
	| equality_expr ne_op relational_expr	 { yypopulate_tree("equality_expr", 3); }
	;

and_expr
	: equality_expr	 //KSV{ yypopulate_tree("and_expr", 1); }
	| and_expr amperstand equality_expr	 { yypopulate_tree("and_expr", 3); }
	;

exclusive_or_expr
	: and_expr	 //KSV{ yypopulate_tree("exclusive_or_expr", 1); }
	| exclusive_or_expr carot and_expr	 { yypopulate_tree("exclusive_or_expr", 3); }
	;

inclusive_or_expr
	: exclusive_or_expr	 //KSV{ yypopulate_tree("inclusive_or_expr", 1); }
	| inclusive_or_expr single_bar exclusive_or_expr	 { yypopulate_tree("inclusive_or_expr", 3); }
	;

logical_and_expr
	: inclusive_or_expr	 //KSV{ yypopulate_tree("logical_and_expr", 1); }
	| logical_and_expr and_op inclusive_or_expr	 { yypopulate_tree("logical_and_expr", 3); }
	;

logical_or_expr
	: logical_and_expr	 //KSV{ yypopulate_tree("logical_or_expr", 1); }
	| logical_or_expr or_op logical_and_expr	 { yypopulate_tree("logical_or_expr", 3); }
	;

conditional_expr
	: logical_or_expr	 //KSV{ yypopulate_tree("conditional_expr", 1); }
	| logical_or_expr qmark logical_or_expr colon conditional_expr	 { yypopulate_tree("conditional_expr", 5); }
	;

assignment_operator
	: equal	 { yypopulate_tree("assignment_operator", 1); }
	| MUL_ASSIGN		{ yypopulate_tree(*($1), 0); }
	| DIV_ASSIGN		{ yypopulate_tree(*($1), 0); }
	| MOD_ASSIGN		{ yypopulate_tree(*($1), 0); }
	| ADD_ASSIGN		{ yypopulate_tree(*($1), 0); }
	| SUB_ASSIGN		{ yypopulate_tree(*($1), 0); }
	| LEFT_ASSIGN			{ yypopulate_tree(*($1), 0); }
	| RIGHT_ASSIGN			{ yypopulate_tree(*($1), 0); }
	| AND_ASSIGN			{ yypopulate_tree(*($1), 0); }
	| XOR_ASSIGN			{ yypopulate_tree(*($1), 0); }
	| OR_ASSIGN		{ yypopulate_tree(*($1), 0); }
	;

assignment_expr
	: conditional_expr	 //KSV{ yypopulate_tree("assignment_expr", 1); }
	| unary_expr assignment_operator assignment_expr	 { yypopulate_tree("assignment_expr", 3); }
	;

constant_expr
	: conditional_expr	 { yypopulate_tree("constant_expr", 1); }
	;

comma
	: ','				{ yypopulate_tree(*($1), 0); } 
	;
period
	: '.'				{ yypopulate_tree(*($1), 0); } 
	;
emark
	: '!'				{ yypopulate_tree(*($1), 0); } 
	;
qmark
	: '?'				{ yypopulate_tree(*($1), 0); } 
	;
semi
	: ';'				{ yypopulate_tree(*($1), 0); } 
	;
colon
	: ':'				{ yypopulate_tree(*($1), 0); } 
	;
lparen
	: '('				{ yypopulate_tree(*($1), 0); } 
	;
rparen
	: ')'				{ yypopulate_tree(*($1), 0); } 
	;
lbracket
	: '['				{ yypopulate_tree(*($1), 0); } 
	;
rbracket
	: ']'				{ yypopulate_tree(*($1), 0); } 
	;
lbrace
	: '{'				{ yypopulate_tree(*($1), 0); in_braces++; } 
	;
rbrace
	: '}'				{ yypopulate_tree(*($1), 0); in_braces--; } 
	;
equal
	: '='				{ yypopulate_tree(*($1), 0); } 
	;
star
	: '*'				{ yypopulate_tree(*($1), 0); } 
	;
plus
	: '+'				{ yypopulate_tree(*($1), 0); } 
	;
minus
	: '-'				{ yypopulate_tree(*($1), 0); } 
	;
slash
	: '/'				{ yypopulate_tree(*($1), 0); } 
	;
carot
	: '^'				{ yypopulate_tree(*($1), 0); } 
	;
amperstand
	: '&'				{ yypopulate_tree(*($1), 0); } 
	;
single_bar
	: '|'				{ yypopulate_tree(*($1), 0); } 
	;
tilda
	: '~'				{ yypopulate_tree(*($1), 0); } 
	;
percent
	: '%'				{ yypopulate_tree(*($1), 0); } 
	;
langle
	: '<'				{ yypopulate_tree(*($1), 0); } 
	;
rangle
	: '>'				{ yypopulate_tree(*($1), 0); } 
	;
sizeof
	: SIZEOF			{ yypopulate_tree(*($1), 0); }
	;
ptr_op
	: PTR_OP			{ yypopulate_tree(*($1), 0); }
	;
inc_op
	: INC_OP			{ yypopulate_tree(*($1), 0); }
	;
dec_op
	: DEC_OP			{ yypopulate_tree(*($1), 0); }
	;
left_op
	: LEFT_OP			{ yypopulate_tree(*($1), 0); }
	;
right_op
	: RIGHT_OP			{ yypopulate_tree(*($1), 0); }
	;
le_op
	: LE_OP			{ yypopulate_tree(*($1), 0); }
	;
ge_op
	: GE_OP			{ yypopulate_tree(*($1), 0); }
	;
eq_op
	: EQ_OP			{ yypopulate_tree(*($1), 0); }
	;
ne_op
	: NE_OP			{ yypopulate_tree(*($1), 0); }
	;
and_op
	: AND_OP			{ yypopulate_tree(*($1), 0); }
	;
or_op
	: OR_OP			{ yypopulate_tree(*($1), 0); }
	;
enum
	: ENUM			{ yypopulate_tree(*($1), 0); in_struct++;}
	;
elipsis
	: ELIPSIS			{ yypopulate_tree(*($1), 0); }
	;
case
	: CASE		{ yypopulate_tree(*($1), 0); }
	;
default
	: DEFAULT		{ yypopulate_tree(*($1), 0); }
	;
if
	: IF		{ yypopulate_tree(*($1), 0); }
	;
else
	: ELSE		{ yypopulate_tree(*($1), 0); }
	;
switch
	: SWITCH		{ yypopulate_tree(*($1), 0); }
	;
while
	: WHILE		{ yypopulate_tree(*($1), 0); }
	;
do
	: DO		{ yypopulate_tree(*($1), 0); }
	;
for
	: FOR		{ yypopulate_tree(*($1), 0); }
	;
goto
	: GOTO		{ yypopulate_tree(*($1), 0); }
	;
continue
	: CONTINUE		{ yypopulate_tree(*($1), 0); }
	;
break
	: BREAK		{ yypopulate_tree(*($1), 0); }
	;
return
	: RETURN		{ yypopulate_tree(*($1), 0); }
	;

%%

void yyerror(char *s)
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}

