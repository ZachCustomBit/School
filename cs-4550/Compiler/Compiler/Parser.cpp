//
//  Parser.cpp
//  Compiler
//
//  Created by Zach on 3/24/15.
//  Copyright (c) 2015 Homework. All rights reserved.
//

#include "Parser.h"
#include "Scanner.h"
#include "SymbolTable.h"
#include "Node.h"

using namespace std;

Parser::Parser(Scanner *scanner, SymbolTable *st)
: mScanner(scanner), mSymbolTable(st) {
    MSG("Parser init");
}

StartNode * Parser::Start() {
    ProgramNode *pn = Program();
    Match(EOF_TOKEN);
    StartNode *sn = new StartNode(pn);
    return sn;
}

Token Parser::Match(TokenType expectedType) {
    Token currentToken = mScanner->GetNextToken();
    if (currentToken.GetTokenType() != expectedType) {
        cerr << "Failed to match Token Type, got: " << currentToken.GetTokenTypeName() << endl;
        exit(1);
    }
    MSG("Sucessfully matched Token Type: " <<
        currentToken.GetTokenTypeName() << ". Lexeme: \"" <<
        currentToken.GetLexeme() << "\"");
    return currentToken;
}

ProgramNode * Parser::Program() {
    Match(VOID_TOKEN);
    Match(MAIN_TOKEN);
    Match(LPAREN_TOKEN);
    Match(RPAREN_TOKEN);
    BlockNode *bn = Block();
    ProgramNode *pn = new ProgramNode(bn);
    return pn;
}

BlockNode * Parser::Block() {
    Match(LCURLY_TOKEN);
    StatementGroupNode *sgn = StatementGroup();
    Match(RCURLY_TOKEN);
    BlockNode *bn = new BlockNode(sgn);
    return bn;
}

StatementGroupNode* Parser::StatementGroup() {
    StatementGroupNode *sgn = new StatementGroupNode();
    while (true) {
        StatementNode *statement = Statement();
        if (statement == NULL) {
            return sgn;
        } else {
            sgn->AddStatement(statement);
        }
    }
}

StatementNode * Parser::Statement() {
    Token nextToken = mScanner->PeekNextToken();
    TokenType tt = nextToken.GetTokenType();
    if (tt == INT_TOKEN) {
        return DeclarationStatement();
    }
    else if (tt == IDENTIFIER_TOKEN) {
        return AssignmentStatement();
    }
    else if (tt == COUT_TOKEN) {
        return CoutStatement();
    }
    else if (tt == LCURLY_TOKEN) {
        return Block();
    }
    else if (tt == IF_TOKEN) {
        return IfStatement();
    }
    else if (tt == DO_TOKEN) {
        return DoWhile();
    }
//    else if (tt == WHILE_TOKEN) {
//        return WhileStatement();
//    }
//    else if (tt == REPEAT_TOKEN) {
//        return RepeatStatement();
//    }
    return NULL;
}

DeclarationStatementNode * Parser::DeclarationStatement() {
    Match(INT_TOKEN);
    IdentifierNode *identifier = Identifier();
    Match(SEMICOLON_TOKEN);
    DeclarationStatementNode* dsn = new DeclarationStatementNode(identifier);
    return dsn;
}

AssignmentStatementNode * Parser::AssignmentStatement() {
    IdentifierNode *identifier = Identifier();
    TokenType tt = mScanner->PeekNextToken().GetTokenType();
    AssignmentStatementNode *asn = NULL;
    if (tt == ASSIGNMENT_TOKEN) {
        Match(ASSIGNMENT_TOKEN);
        ExpressionNode *expression = Expression();
        asn = new AssignmentStatementNode(identifier, expression);
    }
    else if (tt == PLUS_EQUAL_TOKEN) {
        Match(PLUS_EQUAL_TOKEN);
        ExpressionNode *expression = Expression();
        asn = new PlusEqualNode(identifier, expression);
    }
    else if (tt == MINUS_EQUAL_TOKEN) {
        Match(MINUS_EQUAL_TOKEN);
        ExpressionNode *expression = Expression();
        asn = new MinusEqualNode(identifier, expression);
    }
    Match(SEMICOLON_TOKEN);
    return asn;
}

CoutStatementNode * Parser::CoutStatement() {
    Match(COUT_TOKEN);
    Match(INSERTION_TOKEN);
    CoutStatementNode *csn = new CoutStatementNode();
    ExpressionNode *expression = Expression();
    csn->AddExpressionNode(expression);
    while (true) {
        TokenType tt = mScanner->PeekNextToken().GetTokenType();
        if (tt == INSERTION_TOKEN) {
            Match(INSERTION_TOKEN);
            Token next = mScanner->PeekNextToken();
            if (next.GetTokenType() == ENDL_TOKEN) {
                Match(ENDL_TOKEN);
                csn->AddExpressionNode(NULL);
            }
            else {
                ExpressionNode *nextExpression = Expression();
                csn->AddExpressionNode(nextExpression);
            }
        }
        else {
            if (tt == ENDL_TOKEN) {
                Match(ENDL_TOKEN);
                csn->AddExpressionNode(NULL);
            }
            Match(SEMICOLON_TOKEN);
            break;
        }
    }
    return csn;
}

IdentifierNode * Parser::Identifier() {
    Token token = Match(IDENTIFIER_TOKEN);
    IdentifierNode *in = new IdentifierNode(token.GetLexeme(), mSymbolTable);
    return in;
}

ExpressionNode * Parser::Expression() {
    ExpressionNode *en = Or();
    return en;
}

ExpressionNode* Parser::Or() {
    ExpressionNode *current = And();
    while (true) {
        TokenType tt = mScanner->PeekNextToken().GetTokenType();
        if (tt == OR_TOKEN) {
            Match(tt);
            current = new OrNode(current, And());
        }
        else {
            break;
        }
    }
    return current;
}

ExpressionNode* Parser::And() {
    ExpressionNode *current = Relational();
    while (true) {
        TokenType tt = mScanner->PeekNextToken().GetTokenType();
        if (tt == AND_TOKEN) {
            Match(tt);
            current = new AndNode(current, Relational());
        }
        else {
            break;
        }
    }
    return current;
}

ExpressionNode * Parser::Relational() {
    ExpressionNode * current = Side();
    TokenType tt = mScanner->PeekNextToken().GetTokenType();
    if (tt == LESS_TOKEN || tt == LESS_EQUAL_TOKEN ||
        tt == GREATER_TOKEN || tt == GREATER_EQUAL_TOKEN ||
        tt == EQUAL_TOKEN || tt == NOT_EQUAL_TOKEN) {
        Match(tt);
        if (tt == LESS_TOKEN)
            current = new LessNode(current, Relational());
        else if(tt == LESS_EQUAL_TOKEN)
            current = new LessEqualNode(current, Relational());
        else if(tt == GREATER_TOKEN)
            current = new GreaterNode(current, Relational());
        else if(tt == GREATER_EQUAL_TOKEN)
            current = new GreaterEqualNode(current, Relational());
        else if(tt == EQUAL_TOKEN)
            current = new EqualNode(current, Relational());
        else if(tt == NOT_EQUAL_TOKEN)
            current = new NotEqualNode(current, Relational());
    }
    return current;
}

ExpressionNode * Parser::Side() {
    ExpressionNode *current = Term();
    while(true) {
        TokenType tt = mScanner->PeekNextToken().GetTokenType();
        if(tt == PLUS_TOKEN) {
            Match(tt);
            current = new PlusNode(current, Term());
        }
        else if(tt == MINUS_TOKEN) {
            Match(tt);
            current = new MinusNode(current, Term());
        }
        else {
            return current;
        }
    }
}

ExpressionNode* Parser::Term() {
    ExpressionNode *current = Exponent();
    while (true) {
        TokenType tt = mScanner->PeekNextToken().GetTokenType();
        if (tt == TIMES_TOKEN) {
            Match(tt);
            current = new TimesNode(current, Exponent());
        }
        else if(tt == DIVIDE_TOKEN) {
            Match(tt);
            current = new DivideNode(current, Exponent());
        }
        else {
            return current;
        }
    }
}

ExpressionNode* Parser::Exponent() {
    ExpressionNode *current = Factor();
    while (true) {
        TokenType tt = mScanner->PeekNextToken().GetTokenType();
        if (tt == EXPONENT_TOKEN) {
            Match(tt);
            current = new ExponentNode(current, Factor());
        }
        else {
            return current;
        }
    }
}

ExpressionNode* Parser::Factor() {
    ExpressionNode *current = NULL;
    TokenType tt = mScanner->PeekNextToken().GetTokenType();
    if (tt == IDENTIFIER_TOKEN) {
        current = Identifier();
    }
    else if (tt == INTEGER_TOKEN) {
        current = Integer();
    }
    else if (tt == NOT_TOKEN) {
        current = Not();
    }
    else {
        Match(LPAREN_TOKEN);
        current = Expression();
        Match(RPAREN_TOKEN);
    }
    return current;
}

NotNode * Parser::Not() {
    Match(NOT_TOKEN);
    ExpressionNode *current = Factor();
    NotNode *e = new NotNode(current);
    return e;
}

IntegerNode * Parser::Integer() {
    Token tc = Match(INTEGER_TOKEN);
    string s = tc.GetLexeme();
    const char *c = s.c_str();
    IntegerNode *i = new IntegerNode(atoi(c));
    return i;
}

IfStatementNode* Parser::IfStatement() {
    StatementNode* S2 = NULL;
    Match(IF_TOKEN);
    Match(LPAREN_TOKEN);
    ExpressionNode * e = Expression();
    Match(RPAREN_TOKEN);
    StatementNode * S1 = Statement();
    TokenType tt = mScanner->PeekNextToken().GetTokenType();
    if (tt == ELSE_TOKEN) {
        Match(ELSE_TOKEN);
        S2 = Statement();
    }
    IfStatementNode* in = new IfStatementNode(e,S1,S2);
    return in;
}

WhileStatementNode* Parser::WhileStatement() {
    Match(WHILE_TOKEN);
    Match(LPAREN_TOKEN);
    ExpressionNode *e = Expression();
    Match(RPAREN_TOKEN);
    StatementNode *statement = Statement();
    WhileStatementNode *ws = new WhileStatementNode(e, statement);
    return ws;
}

DoWhileNode* Parser::DoWhile() {
    StatementNode *statement = NULL;
    StatementGroupNode *statementgroup = NULL;
    Match(DO_TOKEN);
    TokenType tt = mScanner->PeekNextToken().GetTokenType();
    if (tt == LCURLY_TOKEN) {
        Match(LCURLY_TOKEN);
        statementgroup = StatementGroup();
        Match(RCURLY_TOKEN);
    }
    else {
        statement = Statement();
    }
    Match(WHILE_TOKEN);
    Match(LPAREN_TOKEN);
    ExpressionNode *e = Expression();
    Match(RPAREN_TOKEN);
    Match(SEMICOLON_TOKEN);
    DoWhileNode *dw = new DoWhileNode(e, statement, statementgroup);
    return dw;
}

//RepeatStatementNode* Parser::RepeatStatement() {
//    Match(REPEAT_TOKEN);
//    Match(LPAREN_TOKEN);
//    ExpressionNode *e = Expression();
//    Match(RPAREN_TOKEN);
//    StatementNode *statement = Statement();
//    RepeatStatementNode *rsn = new RepeatStatementNode(e, statement);
//    return rsn;
//}