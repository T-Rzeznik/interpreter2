//**************************  interpreter.cpp   ***********************

#include <cctype>
#include "interpreter.h"
#include <cmath>

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

    // Handle exponentiation
    if (ch == '^') {
        cin >> ch; // Consume the '^' symbol

        // Parse the exponent
        double exponent = factor();

        // Calculate exponentiation
        return pow(var, exponent);
    }

    if (isdigit(ch) || ch == '.') {      // Factor can be a number
         cin.putback(ch);
         cin >> var >> ch;

        // Check for exponentiation after parsing the number
        if (ch == '^') {
            cin >> ch; // Consume the '^' symbol

            // Parse the exponent
            double exponent = factor();

            // Calculate exponentiation
            return pow(var, exponent);
        }
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
    double rightExpression; // Declare rightExpression outside the switch block
    while (true) {
        switch (ch) {
            case '^': // Handle exponentiation
                cin >> ch; 
                rightExpression = expression(); // Initialize rightExpression here
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
    cin >> ch;
    if (isspace(ch))
        cin >> ch;
    readId(id); // Read the first identifier
    strcpy(command, id);
    for (int i = 0; i < strlen(command); i++) 
        command[i] = toupper(command[i]); //already case insensitive?

    if (!hasBeginOccurred && strcmp(command, "BEGIN") != 0) // Check if the first lexeme is "BEGIN"
    {
        issueError("Expected 'BEGIN' as the first lexeme");
        return;
    } 
    else if (!hasBeginOccurred && strcmp(command, "BEGIN") == 0) 
    {
        hasBeginOccurred = true; // Set the flag to true after encountering "BEGIN"
        cout << "BEGIN" << endl; // Output "BEGIN" as a standalone statement
        return;
    }

    if (strcmp(command,"STATUS") == 0)
        cout << *this;
    else if (strcmp(command,"PRINT") == 0) {
        char delimiter;
        bool firstVariable = true; // Track if it's the first variable being printed
        while (true) {
            if (!firstVariable) // If it's not the first variable, read the next identifier
                readId(id);
            if (!findId(id)) // Check if the variable exists
            {
                issueError("Variable not found");
                return;
            }
            if (!firstVariable) // If it's not the first variable, print a comma
                cout << ", ";
            cout << id << " = " << findValue(id); // Print the variable
            firstVariable = false;
            if (!(cin >> delimiter && delimiter == ',')) // Check if there are more variables to print
                break;
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
            if (ch != '$')  //changing terminator $ instead ;
                issueError("There are some extras in the statement");
            else processNode(id,e);
        }
        else issueError("'=' is missing");
    }
}






