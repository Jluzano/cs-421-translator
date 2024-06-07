#include<iostream>
#include<fstream>
#include<string>
#include<stdlib.h>
using namespace std;

/* INSTRUCTION:  copy your parser.cpp here
      cp ../ParserFiles/parser.cpp .
   Then, insert or append its contents into this file and edit.
   Complete all ** parts.
*/

//=================================================
// PARSER CODE
//=================================================

ifstream fin;
ofstream write;

enum tokentype {WORD1, WORD2, PERIOD, ERROR, EOFM, VERB, VERBNEG, VERBPAST, 
                VERBPASTNEG, IS, WAS, OBJECT, SUBJECT, DESTINATION, PRONOUN, CONNECTOR};

// ** For the display names of tokens - must be in the same order as the tokentype.
string tokenName[30] = {"WORD1", "WORD2", "PERIOD", "ERROR", "EOFM", "VERB", "VERBNEG", "VERBPAST", "VERBPASTNEG", "IS", "WAS", "OBJECT","SUBJECT", "DESTINATION", "PRONOUN", "CONNECTOR"};

string reservedWords[19][2] =
{
  {"masu", "VERB"},
  {"masen", "VERBNEG"},
  {"mashita", "VERBPAST"},
  {"masendeshita", "VERBPASTNEG"},
  {"desu", "IS"},
  {"deshita", "WAS"},
  {"o", "OBJECT"},
  {"wa", "SUBJECT"},
  {"ni", "DESTINATION"},
  {"watashi", "PRONOUN"},
  {"anata", "PRONOUN"},
  {"kare", "PRONOUN"},
  {"kanojo", "PRONOUN"},
  {"sore", "PRONOUN"},
  {"shikashi", "CONNECTOR"},
  {"dakara", "CONNECTOR"},
  {"mata", "CONNECTOR"},
  {"soshite", "CONNECTOR"},

  {"eofm", "EOFM"}
};

int scanner(tokentype& tt, string& w);
bool word(string s);
bool period(string s);

string filename;
tokentype saved_token;
string saved_lexeme;
bool token_available = false;

void syntaxerror1(tokentype expected, string lexeme)
{
   cout << "SYNTAX ERROR: expected " << tokenName[expected] << " but found " << lexeme << endl;
   exit(EXIT_FAILURE);
}

void syntaxerror2(string lexeme, string parserFunction)
{
   cout << "SYNTAX ERROR: unexpected " << parserFunction << " found in " << lexeme << endl;
   exit(EXIT_FAILURE);
}

tokentype next_token()
{
	if (!token_available)					 // if there is no saved token yet
	{
		scanner(saved_token, saved_lexeme);	 // call scanner to grab a new token
		cout << "Scanner called using word: " << saved_lexeme << endl;
		if (saved_lexeme == "eofm")
		{
			cout << "Successfully parsed <story>." << endl;
			exit(1);
		}
		token_available = true;				 // mark that fact that you have saved it
	}
	return saved_token;    // return the saved token
}

bool match(tokentype expected)
{
	if (next_token() != expected)  // mismatch has occurred with the next token
	{ // calls a syntax error function here to  generate a syntax error message here and do recovery
		syntaxerror1(expected, saved_lexeme);
	}
	else  // match has occurred
	{
		cout << "Matched " << tokenName[expected] << endl;
		token_available = false;  // eat up the token
		return true;              // say there was a match
	}
    return EXIT_SUCCESS;
}

bool word (string s)
{
  int state = 0;
  int charpos = 0;

  while (s[charpos] != '\0')
  {
    bool vowels = s[charpos] == 'a' || s[charpos] == 'e' || s[charpos] == 'i' || s[charpos] == 'o' || s[charpos] == 'u' || s[charpos] == 'I' || s[charpos] == 'E';
    bool bghkmpr = s[charpos] == 'b' || s[charpos] == 'g' || s[charpos] == 'h' || s[charpos] == 'k' || s[charpos] == 'm' || s[charpos] == 'p' || s[charpos] == 'r';
    bool dwzyj = s[charpos] == 'd' || s[charpos] == 'w' || s[charpos] == 'z' || s[charpos] == 'y' || s[charpos] == 'j';

    if (state == 0 && vowels)
        state = 1;
    else if (state == 0 && s[charpos] == 't')
        state = 4;
    else if (state == 0 && s[charpos] == 's')
        state = 3;
    else if (state == 0 && s[charpos] == 'c')
        state = 2;
    else if (state == 0 && dwzyj)
        state = 6;
    else if (state == 0 && (bghkmpr || s[charpos] == 'n'))
        state = 5;

    else if (state == 5 && vowels)
        state = 1;
    else if (state == 5 && s[charpos] == 'y')
        state = 6;

    else if (state == 4 && vowels)
        state = 1;
    else if (state == 4 && s[charpos] == 's')
        state = 6;

    else if (state == 3 && vowels)
        state = 1;
    else if (state == 3 && s[charpos] == 'h')
        state = 6;

    else if (state == 2 && s[charpos] == 'h')
        state = 6;

    else if (state == 6 && vowels)
        state = 1;

    else if (state == 1 && vowels)
        state = 1;
    else if (state == 1 && s[charpos] == 't')
        state = 4;
    else if (state == 1 && s[charpos] == 's')
        state = 3;
    else if (state == 1 && s[charpos] == 'c')
        state = 2;
    else if (state == 1 && bghkmpr)
        state = 5;
    else if (state == 1 && s[charpos] == 'n')
        state = 7;
    else if (state == 1 && dwzyj)
        state = 6;

    else if (state == 7 && vowels)
        state = 1;
    else if (state == 7 && s[charpos] == 'c')
        state = 2;
    else if (state == 7 && s[charpos] == 's')
        state = 3;
    else if (state == 7 && s[charpos] == 't')
        state = 4;
    else if (state == 7 && (bghkmpr || s[charpos] == 'n'))
        state = 5;
    else if (state == 7 && dwzyj)
        state = 6;
    else
        return false;

    charpos++;
  }//end of while

  return state == 1 || state == 7;
}

bool period(string s)
{
    int state = 0;       // Initialize the state to 0
    int charpos = 0;     // Initialize the character position to 0

    // Loop through each character in the input string
    while (s[charpos] != '\0')
    {
        // Check the current state and character
        if (state == 0 && s[charpos] == '.')
        {
            // If the current state is 0 and the current character is a period, transition to state 1
            state = 1;
        }
        else
        {
            // If the current state is not 0 or the current character is not a period, return false
            return false;
        }
        charpos++;      // Increment the character position
    }

    // If the loop completes and the final state is 1, return true; otherwise, return false
    return state == 1;
}

int scanner(tokentype& tt, string& w)
{
  fin >> w;
  if (w == "eofm") 
    {
      tt = EOFM;
      return 0;              // 1. If it is eofm, return right now.   
    }

  if (word(w)) 
{  
    bool reserved = false;
    int size = sizeof(reservedWords) / sizeof(*reservedWords);
    for (int i = 0; i < size ; i++) 
    {
        // Checking against the reserved words list if it's a word
        if (w == reservedWords[i][0])
        {
            // If it's a reserved word, get the associated token type
            string temp = reservedWords[i][1];
            int num = sizeof(tokenName) / sizeof(tokenName[0]);
            for (int j = 0; j < num; j++)
            {
                // Check the token type against the list of token names
                if (temp == tokenName[j])
                {
                    // If the token type is found, set the token type and mark the word as reserved
                    tt = (tokentype)j;
                    reserved = true;
                    break;
                }
            }
            break;
        }
    }

    if (reserved)
      return 0;
    char back = w[w.size() - 1];
    //Tokentype is WORD1 or WORD2 based on the last character
    if (back == 'I' || back == 'E')
        tt = WORD2;
    else if (back == '*')
    {
        tt = ERROR;
        cout << "Lexical error: " << w << " is not a valid token" << endl;
    }
    else
        tt = WORD1;
  }
  //If the last character is a period
  else if (period(w))
      tt = PERIOD;
  else
  {
      //If both DFAs fail
      tt = ERROR;
      cout << "Lexical error: " << w << " is not a valid token" << endl;
    }
    return 0;

}//the end of scanner

//=================================================
// END OF PARSER CODE
//=================================================

//=================================================
// File translator.cpp written by Group Number: ** 5
//=================================================

// ----- Additions to the parser.cpp ---------------------

// ** Declare Lexicon (i.e. dictionary) that will hold the content of lexicon.txt
// Make sure it is easy and fast to look up the translation.
// Do not change the format or content of lexicon.txt 
//  Done by: ** Justin
string dictionary[53][2] = { " " }; // 2D array to hold dictionary
string saved_E_word; // Holds the English translation of saved_lexeme

// Function to read in the contents of lexicon.txt into the dictionary array
void makeLexicon()
{
    fin.open("lexicon.txt"); // Open lexicon.txt file
    int i = 0;
    while (!fin.eof()) // Read the file until the end
    {
        string japaneseWord, englishWord;
        fin >> japaneseWord;
        fin >> englishWord;
        dictionary[i][0] = japaneseWord; // Japanese word will go into the first column
        dictionary[i][1] = englishWord; // English word will go into the second column
        i++;
    }
    fin.close();
}

void getEword(); // Forward declaration
void gen(string); // Forward declaration

// ** Additions to parser.cpp here:
//    getEword() - using the current saved_lexeme, look up the English word
//                 in Lexicon if it is there -- save the result   
//                 in saved_E_word
//  Done by: Kevin
void getEword()
{
    int rows = sizeof(dictionary) / sizeof(dictionary[0]); // Calculate the number of rows in the dictionary array
    for (int i = 0; i < rows; i++) // Iterate through each row of the dictionary array
    {
        if (dictionary[i][0] == saved_lexeme) // If the Japanese word matches the saved_lexeme
        {
            saved_E_word = dictionary[i][1]; // Save the English translation
            return;
        }
    }
    saved_E_word = saved_lexeme; // If no match was found, save the saved_lexeme as is
}

//    gen(line_type) - using the line type,
//                     sends a line of an IR to translated.txt
//                     (saved_E_word or saved_token is used)
//  Done by: Justin
void gen(string line_type)
{
    if (line_type == "TENSE") // If line_type is TENSE
    {
        string tense = tokenName[saved_token]; // Get the name of the saved token
        write << line_type << ":  " << tense << endl; // Write the tense and its name to translated.txt
        return;
    }
    write << line_type << ":  " << saved_E_word << endl; // Write the line_type and the saved_E_word to translated.txt
}

// ----- Changes to the parser.cpp content ---------------------

// ** Comment update: Be sure to put the corresponding grammar 
//    rule with semantic routine calls
//    above each non-terminal function 

// ** Each non-terminal function should be calling
//    getEword and/or gen now.

//Grammar: <tense> := VERBPAST | VERBPASTNEG | VERB | VERBNEG
void tense()
{
	cout << "Processing <tense>" << endl;
	switch(next_token())
	{
		case VERBPAST:
			match(VERBPAST);
		break;
		case VERBPASTNEG:
			match(VERBPASTNEG);
		break;
		case VERB:
			match(VERB);
		break;
		case VERBNEG:
			match(VERBNEG);
		break;
		default:
			syntaxerror2("tense", saved_lexeme);
		break;
	}
}

// Grammar: <be> ::= IS | WAS
void be()
{
	cout << "Processing <be>" << endl;
	switch(next_token())
	{
		case IS:
		  match(IS);
	  break;
		case WAS:
			match(WAS);
		break;
		default:
			syntaxerror2("be", saved_lexeme);
		break;
	}
}

// Grammar: <verb> ::= WORD2
void verb()
{
	cout << "Processing <verb>" << endl;
	match(WORD2);
}

// Grammar: <noun> ::= WORD1 | PRONOUN
void noun()
{
	cout << "Processing <noun>" << endl;
	switch(next_token())
	{
		case WORD1:
			match(WORD1);
		break;
		case PRONOUN:
			match(PRONOUN);
		break;
		default:
			syntaxerror2("noun", saved_lexeme);
		break;
	}
}

// Grammar: <after object> ::= <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD | <noun> #getEword# DESTINATION #gen(TO)# <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD
//  Done by: ** Kevin
void afterObject()
{
    cout << "Processing <afterObject>" << endl;  // display current production rule

    switch (next_token())
    {
    case WORD2:
        verb();
        getEword();
        gen("ACTION");
        tense();
        gen("TENSE");
        match(PERIOD);  // if next token is a verb, call the verb function, generate ACTION and TENSE and match PERIOD token
        break;
    case WORD1:
        noun();
        getEword();
        match(DESTINATION);  // if next token is a noun, call the noun function and match DESTINATION token
        gen("TO");
        verb();
        getEword();
        gen("ACTION");
        tense();
        gen("TENSE");
        match(PERIOD);  // call the verb function, generate ACTION and TENSE and match PERIOD token
        break;
    case PRONOUN:
        noun();
        getEword();
        match(DESTINATION);  // if next token is a pronoun, call the noun function and match DESTINATION token
        gen("TO");
        verb();
        getEword();
        gen("ACTION");
        tense();
        gen("TENSE");
        match(PERIOD);  // call the verb function, generate ACTION and TENSE and match PERIOD token
        break;
    default:
        syntaxerror2("afterObject", saved_lexeme);  // throw syntax error if none of the above cases are true
        break;
    }
}


// Grammar: <after noun> ::= <be> #gen(DESCRIPTION)# #gen(TENSE)# PERIOD  | DESTINATION #gen(TO)# <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD | OBJECT #gen(OBJECT)# <after object>
//  Done by: ** Kevin
void afterNoun()
{
    cout << "Processing <afterNoun>" << endl;
    switch (next_token())
    {
    case IS:
        be();
        // generate a description
        gen("DESCRIPTION");
        // generate the tense
        gen("TENSE");
        match(PERIOD);
        break;
    case WAS:
        be();
        // generate a description
        gen("DESCRIPTION");
        // generate the tense
        gen("TENSE");
        match(PERIOD);
        break;
    case DESTINATION:
        match(DESTINATION);
        // generate "TO"
        gen("TO");
        verb();
        getEword();
        // generate the action
        gen("ACTION");
        // generate the tense
        tense();
        gen("TENSE");
        match(PERIOD);
        break;
    case OBJECT:
        match(OBJECT);
        // generate "OBJECT"
        gen("OBJECT");
        afterObject();
        break;
    default:
        syntaxerror2("afterNoun", saved_lexeme);
        break;
    }
}


// Grammar: <after subject> ::= <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD | <noun> #getEword# <after noun>
//  Done by: ** Justin & Kevin 
void afterSubject()
{
    // prints out a message indicating that the function is processing <afterSubject>
    cout << "Processing <afterSubject>" << endl;

    // a switch statement that checks the token returned by next_token() function
    switch (next_token())
    {
        // if the token is WORD2:
    case WORD2:
        // call verb() function
        verb();
        // get the English word equivalent of the verb and add it to the generated translation
        getEword();
        gen("ACTION");
        // call tense() function
        tense();
        // add the tense to the generated translation
        gen("TENSE");
        // match the token with PERIOD
        match(PERIOD);
        break;

        // if the token is WORD1:
    case WORD1:
        // call noun() function
        noun();
        // get the English word equivalent of the noun and add it to the generated translation
        getEword();
        // call afterNoun() function
        afterNoun();
        break;

        // if the token is PRONOUN:
    case PRONOUN:
        // call noun() function
        noun();
        // get the English word equivalent of the noun and add it to the generated translation
        getEword();
        // call afterNoun() function
        afterNoun();
        break;

        // if the token is not one of the above three:
    default:
        // call syntaxerror2() function with appropriate error message and the saved_lexeme
        syntaxerror2("afterSubject", saved_lexeme);
        break;
    }
}


// Grammar: <s> ::= [CONNECTOR #getEword# #gen(CONNECTOR)#] <noun> #getEword# SUBJECT #gen(ACTOR)# <after subject>
//  Done by: ** Justin & Kevin 
void callStory()
{
    cout << "Processing <s>" << endl;

    // If the next token is a CONNECTOR, then process it before the noun and subject
    if (next_token() == CONNECTOR)
    {
        match(CONNECTOR); // Match the CONNECTOR token
        getEword(); // Get the English equivalent of the CONNECTOR
        gen("CONNECTOR"); // Generate the CONNECTOR translation
        noun(); // Process the noun
        getEword(); // Get the English equivalent of the noun
        match(SUBJECT); // Match the SUBJECT token
        gen("ACTOR"); // Generate the actor translation
        afterSubject(); // Process the rest of the sentence after the subject
        write << endl; // Write the translated sentence to the output file
    }
    else
    {
        noun(); // Process the noun
        getEword(); // Get the English equivalent of the noun
        match(SUBJECT); // Match the SUBJECT token
        gen("ACTOR"); // Generate the actor translation
        afterSubject(); // Process the rest of the sentence after the subject
        write << endl; // Write the translated sentence to the output file
    }
}


// ---------------- Driver ---------------------------

// The final test driver to start the translator
// Done by:  ** Justin & Kevin
int main()
{
    //** Open the output file for writing
    write.open("translated.txt");

    //** Create the lexicon for translation
    makeLexicon();

    //** Prompt the user for the input file name and open the file
    cout << "Enter the input file name: ";
    cin >> filename;
    fin.open(filename.c_str());

    //** Begin parsing the input file using the <story> rule
    while (true)
        callStory();

    //** Close the input and output files
    fin.close();
    write.close();
}// end
//** require no other input files!
//** syntax error EC requires producing errors.txt of error messages
//** tracing On/Off EC requires sending a flag to trace message output functions

