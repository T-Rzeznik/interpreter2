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
    double f = factor(); // Change to call power() instead of factor()
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
    if (isspace(ch)) // If the first character is a space, move to the next character
        cin >> ch;
    readId(id); // Read the first identifier
    strcpy(command, id);
    for (int i = 0; i < strlen(command); i++) 
        command[i] = toupper(command[i]); // Convert the command to uppercase for case insensitivity

    if (!hasBeginOccurred && strcmp(command, "BEGIN") != 0) // Check if the first lexeme is "BEGIN"
    {
        issueError("Expected 'BEGIN' as the first lexeme");
        return;
    } 
    else if (!hasBeginOccurred && strcmp(command, "BEGIN") == 0) 
    {
        hasBeginOccurred = true; // Set to true after encountering "BEGIN"
        cout << "BEGIN" << endl; // Output "BEGIN" as a standalone statement
        return;
    }

    if (strcmp(command,"STATUS") == 0) // Check if the command is "STATUS"
        cout << *this;
    else if (strcmp(command,"PRINT") == 0) { // Check if the command is "PRINT"
        readId(id);
        cout << id << " = " << findValue(id) << endl;
    }
    else if (strcmp(command,"END") == 0) // Check if the command is "END"
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