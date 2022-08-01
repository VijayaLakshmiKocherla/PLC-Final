/* front.c - a lexical analyzer*/
#include <stdio.h>
#include <ctype.h>

/* Global declarations */
/* Variables */
int charClass;
char lexeme[100];
int token_list[100]; // List of Tokens
char nextChar = ' ';
char prevChar;
int lexLen;
int token;
int nextToken;
int i = 0, j = 0;
FILE *in_fp;

/* Function declarations */
int lookup(char ch);
void addChar(void);
void getChar(void);
void getNonBlank(void);
int lex(void);

// TYPE DECLARATIONS
#define STRING 5
#define INTEGER 6
#define CHARACTER 7
#define FLOAT 8
#define VOID 9

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

// Literals
#define INT_LIT 10	// C-Style Integer Literals
#define FLOATING_LIT 14 // C-Style Floating point literals

/* Token codes */
#define IDENT 11		 // Identifier
#define PRIVATE_IDENT 12 // Private Identifier for Perl scalar variables, arrays and hashmap
#define ASSIGN_OP 20	 // Assignment
#define ADD_OP 21		 // Addition
#define SUB_OP 22		 // Subtraction
#define MULT_OP 23		 // Multiplication
#define DIV_OP 24		 // Division
#define MOD_OP 25		 // Modulo Operator
#define LOGICAL_AND 26	 // Logical And
#define LOGICAL_OR 27	 // Logical Or
#define LOGICAL_NOT 28	 // Logical Not
#define OPEN_BLOCK 29	 // Open Code Block
#define CLOSE_BLOCK 30	 // Close Code Block
#define OPEN_ARRAY 31	 // Open Array Index Parameter
#define CLOSE_ARRAY 32	 // Close Array Index Parameter
#define LEFT_PAREN 33	 // Open Function Parameter
#define RIGHT_PAREN 34	 // Close Function Parameter

/******************************************************/
/* main driver */
int main(void)
{
	/* Open the input data file and process its contents */
	if ((in_fp = fopen("front.in", "r")) == NULL)
		printf("ERROR - cannot open front.in \n");
	else
	{
		getChar();
		do
		{
			lex();
		} while (nextToken != EOF);
	}

	// Printing an ordered list of tokens
	printf("List of Tokens:\n");
	for (j = 0; j <= i; j++)
	{
		printf("%d\n", token_list[j]);
	}
	return 0;
}
/******************************************************/
/* lookup - a function to look up operators and
 parentheses and return the token */
int lookup(char ch)
{
	switch (ch)
	{
	case '_':
		if (prevChar == '$' || prevChar == '@' || prevChar == '#') // Checking if the previous character is $ or @ or #, because PRIVATE perl scalar variable starts with $_,
																   // PRIVATE perl array identifier starts with @_, PRIVATE perl hashmap identifier starts with #_
		{
			addChar();
			nextToken = PRIVATE_IDENT; // Setting nextToken as Private Identifier
		}
		break;

	case '&':
		getChar();
		if (nextChar == '&') // Checking if & is followed by another &
		{
			addChar();				 // adding it to lexeme which makes &&
			nextToken = LOGICAL_AND; // Setting nextToken to LOGICAL_AND for &&
		}

		break;

	case '|':
		getChar();
		if (nextChar == '|') // Checking if | is followed by another |
		{
			addChar();				// adding it to lexeme which makes ||
			nextToken = LOGICAL_OR; // Setting nextToken to LOGICAL_OR for ||
		}
		break;
	case '!':
		addChar();
		nextToken = LOGICAL_NOT;
		break;
	case '(':
		addChar();
		nextToken = LEFT_PAREN;
		break;
	case ')':
		addChar();
		nextToken = RIGHT_PAREN;
		break;
	case '+':
		addChar();
		nextToken = ADD_OP;
		break;
	case '-':
		addChar();
		nextToken = SUB_OP;
		break;
	case '*':
		addChar();
		nextToken = MULT_OP;
		break;
	case '/':
		addChar();
		nextToken = DIV_OP;
		break;
	case '%':
		addChar();
		nextToken = MOD_OP;
		break;
	case '=':
		addChar();
		nextToken = ASSIGN_OP;
		break;
	case '{':
		addChar();
		nextToken = OPEN_BLOCK;
		break;
	case '}':
		addChar();
		nextToken = CLOSE_BLOCK;
		break;
	case '[':
		addChar();
		nextToken = OPEN_ARRAY;
		break;
	case ']':
		addChar();
		nextToken = CLOSE_ARRAY;
		break;
	default:
		addChar();
		nextToken = EOF;
		break;
	}
	return nextToken;
}

/******************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar(void)
{
	if (lexLen <= 98)
	{
		lexeme[lexLen++] = nextChar;
		lexeme[lexLen] = '\0';
	}
	else
		printf("Error - lexeme is too long \n");
}

/******************************************************/
/* getChar - a function to get the next character of
 input and determine its character class */
void getChar(void)
{
	prevChar = nextChar;
	if ((nextChar = getc(in_fp)) != EOF)
	{
		if (isalpha(nextChar))
			charClass = LETTER;
		else if (isdigit(nextChar))
			charClass = DIGIT;
		else if (nextChar == '.')
		{
			charClass = DIGIT;
			nextToken = FLOATING_LIT;
		}
		else if (nextChar == '$') // Perl Scalar variable (IDENTIFIER) which starts with $
			charClass = LETTER;
		else if (nextChar == '@') // Perl Arrays variable (IDENTIFIER) which start with @
			charClass = LETTER;
		else if (nextChar == '#') // Perl HashMap variable (IDENTIFIER) which start with #
			charClass = LETTER;
		else
			charClass = UNKNOWN;
	}
	else
		charClass = EOF;
}

/******************************************************/
/* getNonBlank - a function to call getChar until it
 returns a non-whitespace character */
void getNonBlank(void)
{
	while (isspace(nextChar))
		getChar();
}

/******************************************************/
/* lex - a lexical analyzer */
int lex(void)
{
	lexLen = 0;
	getNonBlank();
	switch (charClass)
	{
		/* Identifiers */
	case LETTER:
		addChar();
		getChar();
		while (charClass == LETTER || charClass == DIGIT)
		{
			addChar();
			getChar();
		}
		if (nextToken != PRIVATE_IDENT)
			nextToken = IDENT;
		break;
		/* Integer literals */
	case DIGIT:
		addChar();
		getChar();
		while (charClass == DIGIT)
		{
			addChar();
			getChar();
		}
		if (nextToken != FLOATING_LIT)
			nextToken = INT_LIT;
		break;
		/* Parentheses, Operators, Special Characters */
	case UNKNOWN:
		lookup(nextChar);
		if (nextToken == PRIVATE_IDENT)
		{
			getChar();
			lex();
		}
		else
			getChar();
		break;
		/* EOF */
	case EOF:
		nextToken = EOF;
		lexeme[0] = 'E';
		lexeme[1] = 'O';
		lexeme[2] = 'F';
		lexeme[3] = '\0';
		break;
	} /* End of switch */

	// Creating an ordered list of tokens
	token_list[i] = nextToken;
	i++;

	printf("Next token is: %d, Next lexeme is %s\n",
		   nextToken, lexeme);

	return nextToken;
} /* End of function lex */
