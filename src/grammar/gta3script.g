grammar gta3script;

// TODO nested comments as in R*'s gta3script
// TODO custom ELSEIF?

options {
	language=C;	// when debugging on ANTLRWorks, this must be commented
	output=AST;
}

tokens {
	BLOCK;	// Block of statements
	SCOPE;	// Scope of statements { }
	LABEL;	// Label:
	ARRAY;	// array[1]
	COMMAND;// COMMAND x y z
	ARGS;
}

@parser::header {
   #define _empty NULL
}

@lexer::header {
   #define _empty NULL
}

////////////////////////////////////////////////////////////////
//************************** Rules **************************//
////////////////////////////////////////////////////////////////

start 
	:	statementList
	;
	
statementList
	:	statement*
	->  ^(BLOCK statement*)
	;
	
statement
	: newLine
	| scopeStatement
	| ifStatement
	| whileStatement
	| repeatStatement
	| switchStatement
	| variableDeclaration
	| loopControlStatement
	| labelStatement
	| commandStatement
	;

labelStatement
	:	WS* IDENTIFIER ':' newLine
	->	^(LABEL IDENTIFIER)
	;

loopControlStatement
	:	WS* (BREAK -> BREAK | CONTINUE -> CONTINUE) newLine
	;

scopeStatement
	:	WS* '{' newLine
			statementList
		WS* '}' newLine
	->	^(SCOPE statementList)
	;
	
//////

variableDeclaration
	:	WS*
		type=(VAR_INT|LVAR_INT|VAR_FLOAT|LVAR_FLOAT|VAR_TEXT_LABEL|LVAR_TEXT_LABEL|VAR_TEXT_LABEL16|LVAR_TEXT_LABEL16)
		variableDeclarationIdentity+
		newLine
	-> ^($type variableDeclarationIdentity+)
	;
	
variableDeclarationIdentity
	:	WS+ IDENTIFIER ('['  integerConstant WS* ']')?
	->  ^(IDENTIFIER integerConstant?)
	;
	
	
//////
	
whileStatement
	:	WS* WHILE conditionList
			statementList
		WS* ENDWHILE newLine
	->  ^(WHILE conditionList statementList)
	;

ifStatement
	:	WS* IF conditionList
			statIf=statementList
		(WS* ELSE newLine
			statElse=statementList)?
		WS* ENDIF newLine
	->	^(IF conditionList $statIf ^(ELSE $statElse)?)
	;
	
repeatStatement
	:	WS* REPEAT argument identifier newLine
			statementList
		WS* ENDREPEAT newLine
	->	^(REPEAT argument identifier statementList)
	;
	
switchStatement
	:	WS* SWITCH argument newLine
			switchStatementCase+
			switchStatementDefault?
		WS* ENDSWITCH newLine
	-> ^(SWITCH argument ^(BLOCK switchStatementCase+ switchStatementDefault?))
	;
	
switchStatementCase
	:	WS* CASE argument newLine statementList
	-> ^(CASE argument statementList)
	;

switchStatementDefault
	:	WS* DEFAULT newLine statementList
	-> ^(DEFAULT statementList)
	;

conditionList
	:	(conditionListSingle|conditionListAnd|conditionListOr)
	;
	
conditionListSingle
	:	commandStatement
	->  ^(commandStatement)
	;
	
conditionListAnd
	:	commandStatement ((WS*)! AND commandStatement)+
	->	^(AND commandStatement+)
	;
	
conditionListOr
	:	commandStatement ((WS*)! OR commandStatement)+
	->	^(OR commandStatement+)
	;

//////

expressionStatement
	:	(id=identifier opa=assignmentOperators a=argument newLine)
		->  ^($opa $id $a)
	|	(id=identifier opr=relationalOperators a=argument newLine)
		->  ^($opr $id $a)
	|	(id=identifier (WS*)! '=' a=argument opb=binaryOperators b=argument newLine)
		->  ^('=' $id ^($opb $a $b))
	|   ((unaryOperators identifier | identifier unaryOperators) newLine)
		->  ^(unaryOperators identifier)
	;

assignmentOperators
	: ((WS*)!
	   op=(
	     '='		// natively supported
	 	|'*='		// natively supported
	 	|'/='		// natively supported
	 	|'%='
	 	|'+=@'		// natively supported
	 	|'-=@'		// natively supported
	 	|'+='		// natively supported
	 	|'-='		// natively supported
	 	|'#='		// natively supported
	 	|'<<='
	 	|'>>='
	 	|'&='
	 	|'^='
	 	|'|='))
	 	-> $op
	;
	
binaryOperators
	: 	((WS*)!
	    op=
	    ('+'	// natively supported
	    |'-'	// natively supported
	    |'*'	// natively supported
	    |'/'	// natively supported
	    |'%'
	    |'+@'	// natively supported
	    |'-@'	// natively supported
	    |'|'
	    |'&'
	    |'^'
	    |'<<'
	    |'>>'))
	    -> $op
	;
	
relationalOperators
	: ((WS*)!
	   op=(
	     '<'
	 	|'>'
	 	|'>='
	 	|'<='))
	 	-> $op
	;

unaryOperators
	:	((WS*)!
		op=('++'|'--'))
		-> $op
	;
	
//////
	
commandStatement
	:	WS* NOT positiveCommandStatement -> ^(NOT positiveCommandStatement)
	|	positiveCommandStatement -> positiveCommandStatement
	;
	
positiveCommandStatement
	:	expressionStatement
	|	(WS* IDENTIFIER argument* newLine) -> ^(COMMAND IDENTIFIER ^(ARGS argument*))
	;

argument
	:	WS*
		(INTEGER -> INTEGER
		|FLOAT -> FLOAT
		|IDENTIFIER '[' id=(IDENTIFIER|INTEGER) WS* ']' -> ^(ARRAY IDENTIFIER $id)
		|IDENTIFIER -> IDENTIFIER
		|LONG_STRING -> LONG_STRING
		|SHORT_STRING -> SHORT_STRING)
	;
	
//////

identifier
	:	WS* IDENTIFIER -> IDENTIFIER
	;

integerConstant
	:	(WS*)! (INTEGER -> INTEGER | FLOAT -> FLOAT | IDENTIFIER -> IDENTIFIER)
	;

newLine
	:	WS* '\r'? ('\n'|EOF) -> '\n'
	;

////////////////////////////////////////////////////////////////
//************************** Tokens **************************//
////////////////////////////////////////////////////////////////

NOT			:	N O T;	
AND			:   A N D ;
OR			:   O R ;
IF			:	I F ;
ELSE		:	E L S E ;
ENDIF		:	E N D I F ;
WHILE		:	W H I L E ;
ENDWHILE	:	E N D W H I L E ;
REPEAT		:	R E P E A T ;
ENDREPEAT	:	E N D R E P E A T ;
SWITCH		:	S W I T C H ;
ENDSWITCH	:	E N D S W I T C H  ;
CASE		:	C A S E ;
DEFAULT		:	D E F A U L T ;
BREAK		:	B R E A K ;
CONTINUE	:	C O N T I N U E ; // custom keyword

VAR_INT   			:	 V A R '_' I N T ;
LVAR_INT  			:	 L V A R '_' I N T ;
VAR_FLOAT   		:	 V A R '_' F L O A T ;
LVAR_FLOAT  		:	 L V A R '_' F L O A T ;
VAR_TEXT_LABEL   	:	 V A R '_' T E X T '_' L A B E L ;
LVAR_TEXT_LABEL  	:	 L V A R '_' T E X T '_' L A B E L ;
VAR_TEXT_LABEL16   	:	 V A R '_' T E X T '_' L A B E L '16' ;
LVAR_TEXT_LABEL16  	:	 L V A R '_' T E X T '_' L A B E L '16' ;


IDENTIFIER  :	('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_'|'.')*
    		;


INTEGER     :   ('-'|'+')? (HEX_LITERAL|OCT_LITERAL|DEC_LITERAL)	
			;

FLOAT		:	('-'|'+')? FLOAT_LITERAL
			;

LONG_STRING	 :  '"' ( ESC_SEQ | ~('\\'|'"') )* '"'
   			 ;

SHORT_STRING :  '\'' ( ESC_SEQ | ~('\\'|'\'') )* '\''
    		 ;

COMMENT
    :   '//' ~('\n'|'\r')* {$channel=HIDDEN;}
    |   '/*' ( options {greedy=false;} : . )* '*/' {$channel=HIDDEN;}
    ;

WS :	(' '|'\t'|')'|'('|',') ;

fragment
HEX_LITERAL : '0' ('x'|'X') HEX_DIGIT+ ;

fragment
OCT_LITERAL : '0' OCT_DIGIT+ ;

fragment
DEC_LITERAL : ('0' | '1'..'9' '0'..'9'*) ;

fragment
FLOAT_LITERAL
    :   ('0'..'9')+ '.' ('0'..'9')* EXPONENT? ('f'|'F')?
    |   '.' ('0'..'9')+ EXPONENT? ('f'|'F')?
    |   ('0'..'9')+ EXPONENT ('f'|'F')?
    ;

fragment
EXPONENT : ('e'|'E') ('+'|'-')? ('0'..'9')+ ;

fragment
DEC_DIGIT :	('0'..'9');

fragment
HEX_DIGIT : ('0'..'9'|'a'..'f'|'A'..'F') ;

fragment
OCT_DIGIT : ('0'..'7');	

fragment
ESC_SEQ
    :   '\\' ('b'|'t'|'n'|'f'|'r'|'\"'|'\''|'\\')
    |   UNICODE_ESC
    |   OCTAL_ESC
    ;

fragment
OCTAL_ESC
    :   '\\' ('0'..'3') ('0'..'7') ('0'..'7')
    |   '\\' ('0'..'7') ('0'..'7')
    |   '\\' ('0'..'7')
    ;

fragment
UNICODE_ESC
    :   '\\' 'u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT
    ;
  
fragment A:('a'|'A');
fragment B:('b'|'B');
fragment C:('c'|'C');
fragment D:('d'|'D');
fragment E:('e'|'E');
fragment F:('f'|'F');
fragment G:('g'|'G');
fragment H:('h'|'H');
fragment I:('i'|'I');
fragment J:('j'|'J');
fragment K:('k'|'K');
fragment L:('l'|'L');
fragment M:('m'|'M');
fragment N:('n'|'N');
fragment O:('o'|'O');
fragment P:('p'|'P');
fragment Q:('q'|'Q');
fragment R:('r'|'R');
fragment S:('s'|'S');
fragment T:('t'|'T');
fragment U:('u'|'U');
fragment V:('v'|'V');
fragment W:('w'|'W');
fragment X:('x'|'X');
fragment Y:('y'|'Y');
fragment Z:('z'|'Z');
