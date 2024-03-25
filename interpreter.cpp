//**************************  interpreter.cpp   ***********************

#include <cctype>
#include "interpreter.h"

double Statement::findValue(char *id) {
    IdNode tmp(id);
    list<IdNode>::iterator i = find(idList.begin(),idList.end(),tmp);
    if (i != idList.end())
         return i->value;
    else issueError("Unknown variable");
    return 0;  // this statement will never be reached;
}

void Statement::processNode(char* id ,double e) {
    IdNode tmp(id,e);
    list<IdNode>::iterator i = find(idList.begin(),idList.end(),tmp);
    if (i != idList.end())
         i->value = e;
    else idList.push_front(tmp);
}

// readId() reads strings of letters and digits that start with
// a letter, and stores them in array passed to it as an actual
// parameter. 
// Examples of identifiers are: var1, x, pqr123xyz, aName, etc.

void Statement::readId(char *id) {
    int i = 0;
    if (isspace(ch))
         cin >> ch;       // skip blanks;
    if (isalpha(ch)) {
         while (isalnum(ch)) {
             id[i++] = ch;
             cin.get(ch); // don't skip blanks;
         }
         id[i] = '\0';
    }
    else issueError("Identifier expected");
}

double Statement::factor() {
    double var, minus = 1.0;
    static char id[200];
    cin >> ch;
    while (ch == '+' || ch == '-') {      // take all '+'s and '-'s.
        if (ch == '-')
            minus *= -1.0;
        cin >> ch;
    }
    if (isdigit(ch) || ch == '.') {      // Factor can be a number
         cin.putback(ch);
         cin >> var >> ch;
    }
    else if (ch == '(') {                  // or a parenthesized expression,
         var = expression();
         if (ch == ')')
              cin >> ch;
         else issueError("Right paren left out");
    }
    else {
         readId(id);                          // or an identifier.
         if (isspace(ch))
             cin >> ch;
         var = findValue(id);
    }
    return minus * var;
}

double Statement::term() {
    double f = factor();
    while (true) {
        switch (ch) {
            case '*' : f *= factor(); break;
            case '/' : f /= factor(); break;
            default  : return f;
        }
    }
}

double Statement::expression() {
    double t = term();
    while (true) {
        switch (ch) {
            case '^': // Handle exponentiation
                cin >> ch; 
                double rightExpression = expression();
                // Evaluate the exponentiation
                t = pow(t, rightExpression);
                break;
            case '+' : t += term(); break;
            case '-' : t -= term(); break;
            default  : return t;
        }
    }
}

bool Statement::findId(char *id) {
    IdNode tmp(id);
    list<IdNode>::iterator i = find(idList.begin(),idList.end(),tmp);
    return i != idList.end();
}

void Statement::getStatement() {
    char id[20], command[20];
    double e;
    cout << "Enter a statement: ";
    cin  >> ch;
    readId(id);
    strcpy(command,id);
    for (int i = 0; i < strlen(command); i++) 
        command[i] = toupper(command[i]); //already case insensitive?

    static bool hasBeginOccurred = false; // track if "BEGIN" has occurred

    if (!hasBeginOccurred) 
    {
        if (strcmp(command, "BEGIN") != 0) { // Check if the first lexeme is "BEGIN"
            issueError("Expected 'BEGIN' as the first lexeme");
            return;
        } 
        else 
        {
            hasBeginOccurred = true; // Set the flag to true after encountering "BEGIN"
        }
    }
    else
    {
        if (strcmp(command, "BEGIN") == 0) // Check if "BEGIN" occurs again after it's already been encountered
        {  
            issueError("Unexpected 'BEGIN' in the middle of the input");
            return; 
        }
    }

    if (strcmp(command,"STATUS") == 0)
        cout << *this;
    else if (strcmp(command,"PRINT") == 0) { //updated handling multiple print: print <identifier> {',' <identifier>} '$'
        char delimiter;
        readId(id);
        cout << id << " = ";
        cout << findValue(id); // Print the first variable
        while (cin >> delimiter && delimiter == ',') {
            readId(id);
            cout << ", " << id << " = ";
            if (!findId(id)) // Check if the variable exists
            {
                issueError("Variable not found");
                return;
            }
            cout << findValue(id); // Print subsequent variables
        }
        cout << endl;
    }
    else if (strcmp(command,"END") == 0)
        exit(0);
    else {
        if (isspace(ch))
            cin >> ch;
        if (ch == '=') {
            e = expression();
            if (ch != '$')  //changing terminator ot $ instead ;
                issueError("There are some extras in the statement");
            else processNode(id,e);
        }
        else issueError("'=' is missing");
    }
}


